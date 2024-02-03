'use strict'

import { PrintInfo, PrintError } from 'zeno';

function ArgListToString(...args) {
    let output = "";
    for (let arg of args) {
        if (arg?.stack) {
            output += arg?.stack;
        } else if (typeof arg == "object") {
            output += `${JSON.stringify(arg)}\t`;
        } else {
            output += `${String(arg)}\t`;
        }
    }
    return output;
}

globalThis.console = {
    log: function(...args) {
        PrintInfo(ArgListToString(...args));
    },
    error: function (...args) {
        PrintError(ArgListToString(...args));
    },
    assert: function (expr, ...args) {
        if (!expr) {
            throw `line ${this.getCurrentLineNumber(3)} assert failed: ${ArgListToString(args)}`;
        }
    },
    getCurrentLineNumber(depth = 2) {
        let error = new Error();
        let stack = error.stack;
        let line = stack.split("\n")?.[depth]?.match(/:(\d+):\d+\)?$/)?.[1];
        return line || -1;
    }
};

globalThis.process = {
    env: {
        NODE_ENV: 'development',
    },
};
