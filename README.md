# Triangle ViewExtension Example

This code uses the SceneViewExtension Class in order to hook the postprocess pipeline.

**Update**
- 5.2 to present there is a Bug with SceneViewExtension hooking a render pass with a delegate hook after the tonemap pass. This will result in a Black Screen in the viewport.
- Current work around for this is to ensure you run Material PostProcess Pass "AfterTonemaper" to ensure the override sequence returns the scenecolor as a input to you render pass until Epic Fixes it.
- Inside PostProcessing.cpp the Bug occurs because of the PassSequence structure overriding the AddAfterPass SceneColor for Anti Alias methods other than NON_AA or FXAA.
