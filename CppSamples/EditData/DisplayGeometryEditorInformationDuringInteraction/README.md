# Display geometry editor information during interaction

Use the geometry editor to see information about the geometry editor's previewed geometry during an editing interaction.

## Use case

The geometry editor can provide information about the geometry being created or edited during an interaction. This information can be used to give feedback to the user to show the effect of the interaction on the geometry.

## How to use the sample

Tap a graphic to edit its geometry by moving, rotating, or scaling the geometry. During the interaction, information about the geometry will be displayed to provide feedback to the user.

Use the buttons in the settings view to undo or redo changes made to the geometry, and use the Discard edits and Save edits buttons to discard or save changes, respectively.

## How it works

1. Create a `GeometryEditor` and set it on the `MapView`.
2. Connect to `GeometryEditor::interactionPreviewChanged` to receive a `GeometryEditorInteractionPreview` during an interaction.
3. Use `GeometryEditorInteractionPreview::previewGeometry` and `GeometryEditorInteractionPreview::interactionType` to display information about move, rotate, and scale interactions.
4. Identify a `Graphic` with `MapView::identifyGraphicsOverlayAsync` and call `GeometryEditor::start` with its geometry.
5. Use `GeometryEditor::undo` and `GeometryEditor::redo` to change the edit history.
6. Call `GeometryEditor::stop` and apply the returned geometry to the edited graphic to save the result.

## Relevant API

* Geometry
* GeometryEditor
* GeometryEditorInteractionPreview
* GeometryEditorInteractionType
* Graphic
* GraphicsOverlay

## Additional information

The `GeometryEditor::interactionPreviewChanged` signal is emitted continuously during an interaction, so it should not be used to trigger resource-intensive operations.

## Tags

draw, edit, geometry editor, interaction preview
