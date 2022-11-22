import crypto from 'crypto';
import fs from 'fs/promises';
import { Config } from '@2003scape/rsc-config';
import { MediaSprites } from '@2003scape/rsc-sprites';
import { createCanvas, createImageData } from 'canvas';
import { MaxRectsPacker } from 'maxrects-packer';

const TEXTURE_SIZE = 1024;

const WHITE_QUAD = { x: 0, y: TEXTURE_SIZE, width: 1, height: 1 };
const TRANSPARENT_QUAD = { x: 1, y: TEXTURE_SIZE, width: 1, height: 1 };

const cacheDirectory = process.argv[2];

if (!cacheDirectory) {
    console.error('missing cache directory for argument 1');
    process.exit(1);
}

const outputDirectory = process.argv[3] || './';

const config = new Config();
await config.init();
config.loadArchive(await fs.readFile(`${cacheDirectory}/config85.jag`));

const mediaSprites = new MediaSprites(config);

// disable full-width sprites
mediaSprites.trim = true;

await mediaSprites.init();

mediaSprites.loadArchive(await fs.readFile(`${cacheDirectory}/media58.jag`));

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

function isCanvasEmpty(canvas) {}

// add metadata for packer
function addSpriteIndex(type) {
    return function (canvas, index) {
        return { type, index, canvas };
    };
}

function formatUV(uv) {
    return `${(uv / TEXTURE_SIZE).toFixed(6)}f`;
}

function toAtlasStruct({ x, y, width, height }) {
    return `    {${formatUV(x)}, ${formatUV(y)}, ${formatUV(width)}, ${formatUV(
        height
    )}},`;
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

        const hash = getHash(sprite.canvas);
        const hashSprite = spriteHashes.get(hash);

        if (hashSprite) {
            duplicates.set(sprite, hashSprite);
            continue;
        }

        const {
            canvas: { width, height }
        } = sprite;

        if (width === 1 && height === 1) {
            continue;
        }

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
                positionTypes[type].length = mediaSprites.idSprites.length;
                positionTypes[type].fill(undefined);
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

    const sprites = [
        ...greySprites.map(addSpriteIndex('grey')),
        ...colouredSprites.map(addSpriteIndex('coloured'))
    ];

    const { positions, canvases } = packSpritesToCanvas(sprites);

    // if there isn't a grey sprite, use a white pixel since it's multiplied
    // by the vertex colour (will multiply by 1, 1, 1, 1)
    const greyPositions = positions.grey.map((sprite) => {
        if (!sprite) {
            return WHITE_QUAD;
        }

        return sprite;
    });

    // if there isn't a coloured sprite, use a transparent pixel since it's
    // added to the vertex colour (will add 0, 0, 0, 0)
    const colouredPositions = positions.coloured.map((sprite) => {
        if (!sprite) {
            return TRANSPARENT_QUAD;
        }

        return sprite;
    });

    const header = [
        '#ifndef _H_MEDIA_TEXTURES',
        '#define _H_MEDIA_TEXTURES',
        '',
        '#include "../../surface.h"',
        '',
        'extern gl_atlas_position gl_media_atlas_positions[];',
        'extern gl_atlas_position gl_media_base_atlas_positions[];',
        '#endif'
    ].join('\n');

    const object = [
        '#include "media.h"',
        '',
        'gl_atlas_position gl_media_atlas_positions[] = {',
        ...greyPositions.map(toAtlasStruct),
        '};',
        '',
        'gl_atlas_position gl_media_base_atlas_positions[] = {',
        ...colouredPositions.map(toAtlasStruct),
        '}'
    ].join('\n');

    //console.log(header, object);
}

await packMedia();
