import crypto from 'crypto';
import fs from 'fs/promises';
import { Config } from '@2003scape/rsc-config';
import { Fonts } from '@2003scape/rsc-fonts';
import { MaxRectsPacker } from 'maxrects-packer';
import { MediaSprites } from '@2003scape/rsc-sprites';
import { createCanvas, createImageData } from 'canvas';

const TEXTURE_SIZE = 1024;

const WHITE_POSITION = { x: 0, y: TEXTURE_SIZE - 1, width: 1, height: 1 };
const TRANSPARENT_POSITION = { x: 1, y: TEXTURE_SIZE - 1, width: 1, height: 1 };

const cacheDirectory = process.argv[2];

if (!cacheDirectory) {
    console.error('missing cache directory for argument 1');
    process.exit(1);
}

const cOutputDirectory = process.argv[3] || './';
const texturesDirectory = process.argv[4] || './';

const config = new Config();
await config.init();
config.loadArchive(await fs.readFile(`${cacheDirectory}/config85.jag`));

const mediaSprites = new MediaSprites(config);

// disable full-width sprites
mediaSprites.trim = true;

await mediaSprites.init();
mediaSprites.loadArchive(await fs.readFile(`${cacheDirectory}/media58.jag`));

const fonts = new Fonts(Fonts.FONTS);
await fonts.init();
fonts.loadArchive(await fs.readFile(`${cacheDirectory}/fonts1.jag`));

// used to determine duplicates
function getHash(image) {
    const { width, height } = image;
    const imageContext = image.getContext('2d');
    const imageData = imageContext.getImageData(0, 0, width, height).data;

    return crypto.createHash('md5').update(imageData).digest('hex');
}

function isGrey(r, g, b, a) {
    return a !== 0 && r !== 0 && r === g && g === b;
}

// add metadata for packer
function addSpriteIndex(type) {
    return function (canvas, index) {
        return { type, index, canvas };
    };
}

function formatUV(uv) {
    return `${(uv / TEXTURE_SIZE).toFixed(6)}f`;
}

function toAtlasStructC({ x, y, width, height }) {
    return `    {${formatUV(x)}, ${formatUV(y)}, ${formatUV(width)}, ${formatUV(
        height
    )}},`;
}

async function writeHeaderC(name, members) {
    name = name.toUpperCase();

    const header = [
        `#ifndef _H_${name}_TEXTURES`,
        `#define _H_${name}_TEXTURES`,
        '',
        '#include "../../surface.h"',
        '',
        ...members.map((member) => `extern ${member};`),
        '#endif'
    ].join('\n');

    name = name.toLowerCase();

    await fs.writeFile(`${cOutputDirectory}/${name}.h`, header);
}

// members = struct definitions
// { 'gl_atlas_position gl_media_atlas_positions': [{x, y, width, height}] }
async function writeMediaC(name, members) {
    //await writeHeaderC(name, Object.keys(members));

    name = name.toLowerCase();

    const object = [
        `#include "${name}.h"`,
        '',
        ...Object.entries(members).map(([memberName, positions]) => {
            return (
                `${memberName} = {\n` +
                positions.map(toAtlasStructC).join('\n') +
                '\n};\n'
            );
        })
    ].join('\n');

    await fs.writeFile(`${cOutputDirectory}/${name}.c`, object);
}

function drawCharacter(canvas, bitmap, colour, xOffset, yOffset) {
    const context = canvas.getContext('2d');
    context.fillStyle = colour;

    for (let y = 0; y < bitmap.length; y++) {
        for (let x = 0; x < bitmap[0].length; x++) {
            if (bitmap[y][x]) {
                context.fillRect(x + xOffset, yOffset + y, 1, 1);
            }
        }
    }
}

// remove coloured (non-grey) pixels and return a new canvas with only
// coloured pixels
function createColouredCanvas(canvas) {
    const spriteContext = canvas.getContext('2d');

    const spriteData = spriteContext.getImageData(
        0,
        0,
        canvas.width,
        canvas.height
    );

    const colouredSprite = createCanvas(canvas.width, canvas.height);
    const colouredSpriteContext = colouredSprite.getContext('2d');

    const colouredSpriteData = colouredSpriteContext.createImageData(
        canvas.width,
        canvas.height
    );

    let hasGrey = false;
    let hasColour = false;

    for (let i = 0; i < canvas.width * canvas.height * 4; i += 4) {
        const [r, g, b, a] = spriteData.data.slice(i, i + 4);

        if (isGrey(r, g, b, a)) {
            hasGrey = true;
        } else {
            if (a !== 0) {
                hasColour = true;
            }

            colouredSpriteData.data[i] = r;
            colouredSpriteData.data[i + 1] = g;
            colouredSpriteData.data[i + 2] = b;
            colouredSpriteData.data[i + 3] = a;
        }
    }

    if (hasGrey && hasColour) {
        colouredSpriteContext.putImageData(colouredSpriteData, 0, 0);

        for (let i = 0; i < canvas.width * canvas.height * 4; i += 4) {
            const [r, g, b, a] = spriteData.data.slice(i, i + 4);

            if (!isGrey(r, g, b, a)) {
                spriteData.data[i] = 0;
                spriteData.data[i + 1] = 0;
                spriteData.data[i + 2] = 0;
                spriteData.data[i + 3] = 0;
            }
        }

        spriteContext.putImageData(spriteData, 0, 0);

        return colouredSprite;
    }
}

// sprites = [ { type, index, canvas } ]
function packSpritesToCanvas(sprites) {
    // { spriteHash: sprite }
    const spriteHashes = new Map();

    // { file: file }
    const duplicates = new Map();

    const toPack = [];

    for (const sprite of sprites) {
        if (!sprite || !sprite.canvas) {
            continue;
        }

        const {
            canvas: { width, height }
        } = sprite;

        if (width <= 0 && height <= 0) {
            continue;
        }

        const hash = getHash(sprite.canvas);
        const hashSprite = spriteHashes.get(hash);

        /*if (hashSprite) {
            duplicates.set(sprite, hashSprite);
            continue;
        }*/

        toPack.push({ sprite, width, height });

        spriteHashes.set(hash, sprite);
    }

    const packer = new MaxRectsPacker(TEXTURE_SIZE, TEXTURE_SIZE, 0);
    packer.addArray(toPack);

    const positions = packer.bins.map(({ rects }) => rects);
    const positionTypes = {};

    const canvases = positions.map((positions) => {
        const textureCanvas = createCanvas(TEXTURE_SIZE, TEXTURE_SIZE);
        const textureContext = textureCanvas.getContext('2d');

        for (const {
            x,
            y,
            width,
            height,
            sprite: { type, index, canvas }
        } of positions) {
            if (positionTypes[type]) {
                positionTypes[type][index] = { x, y, width, height };
            } else {
                positionTypes[type] = [];
            }

            const context = canvas.getContext('2d');

            const imageData = createImageData(
                context.getImageData(0, 0, width, height).data,
                canvas.width
            );

            textureContext.putImageData(imageData, x, y);
        }

        return textureCanvas;
    });

    return { positions: positionTypes, canvases };
}

function toFontArray(positions) {
    positions = positions.map((sprite) => sprite || TRANSPARENT_POSITION);

    return (
        '    {\n' +
        positions
            .map((sprite) => `    ${toAtlasStructC(sprite)}`)
            .join('\n') +
        '\n    },'
    );
}

async function packMedia() {
    // sprites that the client applies a mask to
    const maskedSprites = new Set();

    // undefined here needs to be white
    const greySprites = [];

    // undefined here needs to be transparent
    const colouredSprites = [];

    for (const { sprite, colour } of config.items) {
        if (colour) {
            maskedSprites.add(sprite);
        }
    }

    for (let i = 0; i < mediaSprites.idSprites.length; i++) {
        const sprite = mediaSprites.idSprites[i];

        // handle creating grey masks for inventory item sprites
        if (maskedSprites.has(i - MediaSprites.OBJECTS_OFFSET)) {
            const colouredSprite = createColouredCanvas(sprite);

            if (colouredSprite) {
                colouredSprites[i] = colouredSprite;
            }
        }

        greySprites[i] = sprite;
    }

    const fontSprites = [];

    for (const [i, font] of fonts.fonts.entries()) {
        let index = 0;

        for (const character of Fonts.CHARSET) {
            const {
                bitmap,
                boundingBox: { width, height }
            } = font.getGlyph(character);

            const canvas = createCanvas(width, height);

            drawCharacter(canvas, bitmap, '#f0f', 0, 0);

            fontSprites.push({
                type: `glyph-${i}`,
                index,
                width,
                height,
                canvas
            });

            const shadowCanvas = createCanvas(width + 1, height + 1);

            drawCharacter(shadowCanvas, bitmap, '#000', 1, 0);
            drawCharacter(shadowCanvas, bitmap, '#000', 0, 1);
            drawCharacter(shadowCanvas, bitmap, '#f0f', 0, 0);

            fontSprites.push({
                type: `glyph-shadow-${i}`,
                index,
                width: width + 1,
                height: height + 1,
                canvas: shadowCanvas
            });

            index += 1;
        }
    }

    const sprites = [
        ...greySprites.map(addSpriteIndex('grey')),
        ...colouredSprites.map(addSpriteIndex('coloured')),
        ...fontSprites
    ];

    const { positions, canvases } = packSpritesToCanvas(sprites);

    positions.grey.length = mediaSprites.idSprites.length;
    positions.coloured.length = mediaSprites.idSprites.length;

    for (const [type, typePositions] of Object.entries(positions)) {
        positions[type] = Array.from(typePositions);
    }

    // if there isn't a grey sprite, use a white pixel since it's multiplied
    // by the vertex colour (will multiply by 1, 1, 1, 1)
    const greyPositions = positions.grey.map(
        (sprite) => sprite || WHITE_POSITION
    );

    // if there isn't a coloured sprite, use a transparent pixel since it's
    // added to the vertex colour (will add 0, 0, 0, 0)
    const colouredPositions = positions.coloured.map(
        (sprite) => sprite || TRANSPARENT_POSITION
    );

    //const fontPositions = positions.grey.map((sprite) => sprite || WHITE_QUAD);
    const mediaMembersC = {
        'gl_atlas_position gl_media_atlas_positions[]': greyPositions,
        'gl_atlas_position gl_media_base_atlas_positions[]': colouredPositions
    };

    await writeHeaderC('media', Object.keys(mediaMembersC));
    await writeMediaC('media', mediaMembersC);

    const fontPositions = [];
    const fontShadowPositions = [];

    const totalFonts = fonts.fonts.length;
    const totalChars = Fonts.CHARSET.length;

    for (let i = 0; i < totalFonts; i++) {
        const glyphs = positions[`glyph-${i}`];
        glyphs.length = totalChars;

        fontPositions.push(Array.from(glyphs));

        const shadowGlyphs = positions[`glyph-shadow-${i}`];
        shadowGlyphs.length = Fonts.CHARSET.length;

        fontShadowPositions.push(Array.from(shadowGlyphs));
    }

    await writeHeaderC('fonts', [
        `gl_atlas_position gl_font_atlas_positions[${totalFonts}][${totalChars}]`,
        `gl_atlas_position gl_font_shadow_atlas_positions[${totalFonts}][${totalChars}]`
    ]);

    const object = [
        '#include "fonts.h"',
        '',
        `gl_atlas_position gl_font_atlas_positions[${totalFonts}]` +
            `[${totalChars}] = {`,
        ...fontPositions.map(toFontArray),
        '};',
        '',
        `gl_atlas_position gl_font_shadow_atlas_positions[${totalFonts}]` +
            `[${totalChars}] = {`,
        ...fontShadowPositions.map(toFontArray),
        '};'
    ].join('\n');

    await fs.writeFile(`${cOutputDirectory}/fonts.c`, object);

    const context = canvases[0].getContext('2d');

    context.fillStyle = '#fff';
    context.fillRect(0, TEXTURE_SIZE - 1, 1, 1);

    await fs.writeFile(
        `${texturesDirectory}/sprites.png`,
        canvases[0].toBuffer()
    );
}

await packMedia();
