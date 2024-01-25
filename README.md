# ZenoQuickJS

A simple QuickJS for Unreal Engine.

## Packaging

You can place your script anywhere in your project. Call `FZenoQuickJSModule::AddScriptSearchPath()` to add your search path.

But if you want to package you project, you need to do such steps:

1. Open `Project Settings > Packaging`, Find `Packaging > Advanced > Additional Asset Directories to Cook`.
2. Add your folder and `/ZenoQuickJS/Scripts`.
