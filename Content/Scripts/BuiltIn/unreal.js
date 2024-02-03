'use strict'

import * as unreal from 'unreal'

globalThis.unreal = {
    newObject: function (unrealClass, outerObject = undefined) {
        console.assert(unrealClass.IsValid, "newObject: Invalid ");
        outerObject = outerObject || unreal.GetTransientPackage();
        return unreal.NewObject(outerObject, unrealClass);
    },
};
