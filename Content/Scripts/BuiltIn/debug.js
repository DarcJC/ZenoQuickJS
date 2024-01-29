import { PrintInfo, PrintError } from 'zeno';

function ArgListToString(...args) {
    let output = "";
    for (let arg of args) {
        if (typeof arg == "object") {
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
};

globalThis.process = {
    env: {
        NODE_ENV: 'development',
    },
};
