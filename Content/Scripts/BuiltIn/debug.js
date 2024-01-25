import { println } from 'zeno';

globalThis.console = {
    log: function(...args) {
        let output = "";
        for (let arg of args) {
            if (typeof arg == "object") {
                output += `${JSON.stringify(arg)}\t`;
            } else {
                output += `${String(arg)}\t`;
            }
        }
        println(output);
    },
};
