# Triangle ViewExtension Example

This code uses the SceneViewExtension Class in order to hook the postprocess pipeline.

**Update**
- 5.2 to present there is a Bug with SceneViewExtension hooking a render pass with a delegate hook after the tonemap pass. This will result in BlackScreen.
- Current work around for this is to ensure to add a Material PostProcess Pass "AfterTonemaper" until Epic Fixes it.
- Inside PostProcessing.cpp the Bug occurs for the PassSequence AddAfterPass overriding the SceneColor when it shouldn't for cases of Anti Alias Methods other than NON_AA or FXAA.
