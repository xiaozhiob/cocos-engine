/*
 Copyright (c) 2019 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

/**
 * @category core/value-types
 */

import * as js from '../utils/js';

export function BitMask<T> (obj: T): T {
    if ('__bitmask__' in obj) {
        return obj;
    }
    js.value(obj, '__bitmask__', null, true);

    let lastIndex: number = -1;
    const keys: string[] = Object.keys(obj);

    for (let i = 0; i < keys.length; i++) {
        const key = keys[i];
        let val = obj[key];
        if (val === -1) {
            val = ++lastIndex;
            obj[key] = val;
        }
        else {
            if (typeof val === 'number') {
                lastIndex = val;
            }
            else if (typeof val === 'string' && Number.isInteger(parseFloat(key))) {
                continue;
            }
        }
        const reverseKey: string = '' + val;
        if (key !== reverseKey) {
            if ((CC_EDITOR || CC_TEST) && reverseKey in obj && obj[reverseKey] !== key) {
                cc.errorID(7100, reverseKey);
                continue;
            }
            js.value(obj, reverseKey, key);
        }
    }
    return obj;
}

BitMask.isBitMask = (BitMaskType) => {
    return BitMaskType && BitMaskType.hasOwnProperty('__bitmask__');
};

BitMask.getList = (BitMaskDef) => {
    if (BitMaskDef.__bitmask__) {
        return BitMaskDef.__bitmask__;
    }

    const bitlist: any[] = BitMaskDef.__bitmask__ = [];
    // tslint:disable-next-line: forin
    for (const name in BitMaskDef) {
        const value = BitMaskDef[name];
        if (Number.isInteger(value)) {
            bitlist.push({ name, value });
        }
    }
    bitlist.sort((a, b) => a.value - b.value);
    return bitlist;
};

export function ccbitmask (bitmaskx) {
    if ('__bitmask__' in bitmaskx) {
        return;
    }
    js.value(bitmaskx, '__bitmask__', null, true);
}

cc.BitMask = BitMask;
