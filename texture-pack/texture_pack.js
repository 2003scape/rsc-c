import crypto from 'crypto';
import fs from 'fs/promises';
import url from 'url';
import { Config } from '@2003scape/rsc-config';
import { EntitySprites, MediaSprites } from '@2003scape/rsc-sprites';
import { Fonts } from '@2003scape/rsc-fonts';
import { MaxRectsPacker } from 'maxrects-packer';
import { createCanvas, createImageData } from 'canvas';
import { cssColor, hex2rgb, rgb2hex } from '@swiftcarrot/color-fns';

const __dirname = url.fileURLToPath(new URL('.', import.meta.url));

const TEXTURE_SIZE = 1024;

const ZERO_POSITION = { x: 0, y: 0, width: 0, height: 0 };

//const WHITE_POSITION = { x: 0.5, y: (TEXTURE_SIZE - 1) + 0.5, width: 0.5, height: 0.5 };
const WHITE_POSITION = { x: 0, y: TEXTURE_SIZE - 1, width: 1, height: 1 };

const TRANSPARENT_POSITION = { x: 2, y: TEXTURE_SIZE - 1, width: 1, height: 1 };

// animation names that include skin colour
const SKIN_ANIMATIONS = new Set([
    'body1',
    'fbody1',
    'fhead1',
    'fplatemailtop',
    'head1',
    'head2',
    'head3',
    'head4'
]);

const PLAYER_SKINS = [0xecded0, 0xccb366, 0xb38c40, 0x997326, 0x906020].map(
    (hex) => {
        hex = '#' + hex.toString(16);
        const { r, g, b } = hex2rgb(hex);
        return `rgb(${r}, ${g}, ${b})`;
    }
);

const ENTITY_HEADER = await fs.readFile(
    `${__dirname}/entities.template.h`,
    'utf8'
);

const ENTITY_OBJECT = await fs.readFile(
    `${__dirname}/entities.template.c`,
    'utf8'
);

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

const entitySprites = new EntitySprites(config);

entitySprites.trim = true;

await entitySprites.init();
entitySprites.loadArchive(await fs.readFile(`${cacheDirectory}/entity24.jag`));
entitySprites.loadArchive(await fs.readFile(`${cacheDirectory}/entity24.mem`));

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

function formatUV(uv) {
    return `${(uv / TEXTURE_SIZE).toFixed(6)}f`;
}

function toAtlasStructC({ x, y, width, height }) {
    const leftU = x;
    const rightU = x + width;

    const topV = y;
    const bottomV = y + height;

    return `    {${formatUV(leftU)}, ${formatUV(rightU)}, ${formatUV(
        topV
    )}, ${formatUV(bottomV)}},`;
}

function toEntityStructC(position) {
    if (!position) {
        position = ZERO_POSITION;
        position.canvasIndex = -1;
    }

    const atlas = toAtlasStructC(position)
        .replace('    ', '')
        .replace('},', '}');

    return `    {${position.canvasIndex}, ${atlas}},`;
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
    // { hash: [sprite] }
    const duplicateSprites = new Map();

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
        const duplicates = duplicateSprites.get(hash);

        if (duplicates) {
            duplicates.push(sprite);
            continue;
        } else {
            duplicateSprites.set(hash, []);
        }

        toPack.push({ sprite, width, height, hash });
    }

    const packer = new MaxRectsPacker(TEXTURE_SIZE, TEXTURE_SIZE, 4);
    packer.addArray(toPack);

    const positions = packer.bins.map(({ rects }) => rects);
    const positionTypes = {};

    const canvases = positions.map((positions, canvasIndex) => {
        const textureCanvas = createCanvas(TEXTURE_SIZE, TEXTURE_SIZE);
        const textureContext = textureCanvas.getContext('2d');

        for (const {
            x,
            y,
            width,
            height,
            sprite: { type, index, canvas },
            hash
        } of positions) {
            if (!positionTypes[type]) {
                positionTypes[type] = [];
            }

            positionTypes[type][index] = { x, y, width, height, canvasIndex };

            const duplicates = duplicateSprites.get(hash);

            for (const duplicateSprite of duplicates) {
                if (!positionTypes[duplicateSprite.type]) {
                    positionTypes[duplicateSprite.type] = [];
                }

                positionTypes[duplicateSprite.type][duplicateSprite.index] = {
                    x,
                    y,
                    width,
                    height,
                    canvasIndex
                };
            }

            if (duplicates.length) {
                positionTypes[type];
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
        positions.map((sprite) => `    ${toAtlasStructC(sprite)}`).join('\n') +
        '\n    },'
    );
}

async function packMedia() {
    // sprites that the client applies a mask to
    const maskedSprites = new Set();

    for (const { sprite, colour } of config.items) {
        if (colour) {
            maskedSprites.add(sprite);
        }
    }

    const sprites = [];

    for (let i = 0; i < mediaSprites.idSprites.length; i++) {
        const sprite = mediaSprites.idSprites[i];

        // handle creating grey masks for inventory item sprites
        if (maskedSprites.has(i - MediaSprites.OBJECTS_OFFSET)) {
            const colouredSprite = createColouredCanvas(sprite);

            if (colouredSprite) {
                sprites.push({
                    type: 'coloured',
                    index: i,
                    canvas: colouredSprite
                });
            }
        }

        sprites.push({ type: 'grey', index: i, canvas: sprite });
    }

    for (const [i, font] of fonts.fonts.entries()) {
        let index = 0;

        for (const character of Fonts.CHARSET) {
            const {
                bitmap,
                boundingBox: { width, height }
            } = font.getGlyph(character);

            const canvas = createCanvas(width, height);

            drawCharacter(canvas, bitmap, '#fff', 0, 0);

            sprites.push({
                type: `glyph-${i}`,
                index,
                width,
                height,
                canvas
            });

            const shadowCanvas = createCanvas(width + 1, height + 1);

            drawCharacter(shadowCanvas, bitmap, '#000', 1, 0);
            drawCharacter(shadowCanvas, bitmap, '#000', 0, 1);
            drawCharacter(shadowCanvas, bitmap, '#fff', 0, 0);

            sprites.push({
                type: `glyph-shadow-${i}`,
                index,
                width: width + 1,
                height: height + 1,
                canvas: shadowCanvas
            });

            index += 1;
        }
    }

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
        shadowGlyphs.length = totalChars;

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
    context.fillRect(0, TEXTURE_SIZE - 1, 2, 1);

    // TODO circle

    await fs.writeFile(
        `${texturesDirectory}/sprites.png`,
        canvases[0].toBuffer()
    );
}

async function packEntities() {
    const { npcs } = config;

    // { skinColour: animationNames }
    const skinColourAnimations = new Map();

    // add all of the skin animations for player skin colours
    for (const skinColour of PLAYER_SKINS) {
        skinColourAnimations.set(skinColour, new Set(SKIN_ANIMATIONS));
    }

    const maskedAnimations = new Set();

    for (const animation of config.animations) {
        const { r, g, b } = cssColor(animation.colour);

        if (r !== 0 || g !== 0 || b !== 0) {
            maskedAnimations.add(animation.name.toLowerCase());
        }
    }

    for (let { animations, skinColour } of npcs) {
        if (!skinColour) {
            continue;
        }

        if (skinColour === `rgb(${0xea}, ${0xde}, ${0xd2})`) {
            skinColour = `rgb(${0xec}, ${0xde}, ${0xd0})`;
        } else if (skinColour === `rgb(${0xec}, ${0xff}, ${0xd0})`) {
            skinColour = `rgb(${0xec}, ${0xfe}, ${0xd0})`;
        }

        const animationNames = animations.map((id) =>
            id ? config.animations[id].name : undefined
        );
        const mapAnimations = skinColourAnimations.get(skinColour) || new Set();

        for (const animation of animationNames) {
            if (SKIN_ANIMATIONS.has(animation)) {
                mapAnimations.add(animation.toLowerCase());
            }
        }

        skinColourAnimations.set(skinColour, mapAnimations);
    }

    // used for C array
    const skinColours = Array.from(skinColourAnimations.keys());
    let skinSpriteIDs = new Set();

    let id = 0;
    let sprites = [];

    for (const [animation, frames] of entitySprites.sprites) {
        let currentID = id;

        for (const frame of frames) {
            if (maskedAnimations.has(animation.toLowerCase())) {
                const colouredSprite = createColouredCanvas(frame);

                if (colouredSprite) {
                    sprites.push({
                        index: currentID,
                        type: 'coloured',
                        canvas: colouredSprite
                    });

                    for (const [
                        skinColour,
                        skinAnimations
                    ] of skinColourAnimations) {
                        if (!skinAnimations.has(animation.toLowerCase())) {
                            continue;
                        }

                        const skinColouredSprite =
                            entitySprites.colourizeSprite(
                                colouredSprite,
                                null,
                                skinColour
                            );

                        sprites.push({
                            index: currentID,
                            type: `skin-${skinColours.indexOf(skinColour)}`,
                            canvas: skinColouredSprite
                        });

                        skinSpriteIDs.add(currentID);
                    }
                }
            }

            sprites.push({ index: currentID, type: 'grey', canvas: frame });

            currentID += 1;
        }

        id += 27;
    }

    skinSpriteIDs = Array.from(skinSpriteIDs);

    const { positions, canvases } = packSpritesToCanvas(sprites);

    positions.grey.length = 2000;
    positions.coloured.length = 2000;

    for (const [type, typePositions] of Object.entries(positions)) {
        positions[type] = Array.from(typePositions);
    }

    for (const [i, canvas] of Object.entries(canvases)) {
        await fs.writeFile(
            `${texturesDirectory}/entities_${i}.png`,
            canvas.toBuffer()
        );
    }

    const entityHeader = ENTITY_HEADER.replace(
        '$skin_sprite_length',
        skinSpriteIDs.length
    )
        .replace('$skin_colour_length', skinColours.length)
        .replace('$entity_texture_length', canvases.length);

    await fs.writeFile(`${cOutputDirectory}/entities.h`, entityHeader);

    const skinLines = [];

    for (let i = 0; i < skinColours.length; i++) {
        skinLines.push('    {');

        positions[`skin-${i}`].length = skinSpriteIDs.length;

        for (const position of Array.from(positions[`skin-${i}`])) {
            skinLines.push('    ' + toEntityStructC(position));
        }

        skinLines.push('    },');
    }

    const entityObject = ENTITY_OBJECT.replace(
        '$skin_sprites',
        skinSpriteIDs.join(', ')
    )
        .replace(
            '$skin_colours',
            skinColours
                .map((rgb) => {
                    const { r, g, b } = cssColor(rgb);
                    return rgb2hex(r, g, b).replace('#', '0x');
                })
                .join(', ')
        )
        .replace(
            '$positions',
            Array.from(positions.grey).map(toEntityStructC).join('\n')
        )
        .replace(
            '$base_positions',
            Array.from(positions.coloured).map(toEntityStructC).join('\n')
        )
        .replace('$skin_positions', skinLines.join('\n'));

    await fs.writeFile(`${cOutputDirectory}/entities.c`, entityObject);
}

await packMedia();
await packEntities();
