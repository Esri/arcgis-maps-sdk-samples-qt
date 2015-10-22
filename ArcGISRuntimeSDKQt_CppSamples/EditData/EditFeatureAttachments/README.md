#Edit feature attachments

This sample demonstrates how to edit feature attachments from a feature service using a ServiceFeatureTable.

![](capture.png)

##How it works

This sample calls identify on the MapView in order to obtain features from the touch event. Once a feature is selected, the feature's AttachmentInfo is requested through the fetchAttachmentInfo method. Once the AttachmentInfo is obtained, any existing attachments are added to the combo box. Selecting an attachment from the combo box and then pressing the view button will request the attachment data, and display the resulting image in a message box. To add a new attachment to the feature, addAttachment is called on the feature's AttachmentListModel. To apply this newly added attachment to the service, applyEdits must be called on the ServiceFeatureTable once the addAttachment method completes successfully. To delete an attachment, select an attachment in the combo box, and press the delete button. This will call deleteAttachment on the feature's AttachmentListModel. Similar to addAttachment, you must call applEdits on the ServiceFeatureTable once the addAttachment method completes successfully in order for the edit to be pushed up to the feature service.

##Features
- MapView
- Map
- Basemap
- ServiceFeatureTable
- FeatureLayer
- Feature
- AttachmentListModel
- AttachmentInfo