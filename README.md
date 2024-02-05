# Triangle ViewExtension Example

This code uses the SceneViewExtension Class in order to hook the postprocess pipeline.

**Update**
- 5.2 to present there is a Bug with SceneViewExtension hooking a render pass with a delegate hook after the tonemap pass. This will result in a Black Screen in the viewport.
- Current work around for this is to ensure that you run a Material PostProcess Pass at "AfterTonemaper", this ensure the pass sequence override returns a scenecolor as a input to your render pass.
- Inside PostProcessing.cpp the Bug occurs because of the PassSequence structure overriding the AddAfterPass SceneColor for Anti Alias methods other than NON_AA or FXAA.
