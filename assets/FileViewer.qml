import bb.cascades 1.0
import bb.cascades.pickers 1.0
import bb.system 1.0

// Use a Sheet to hold contents
Sheet {

    // Set properties
    id: fileViewerSheet
    peekEnabled: false
    
    // Create the main page
    Page {
        
        // Set properties
        id: fileViewerPage
        
        // Create title bar
        titleBar: TitleBar {
            
            // Set properties
            title: "Manage Pictures"
            
            // Create done action
            acceptAction: ActionItem {
                
                // Set properties
                title: "Done"
                
                // Handler for trigger
                onTriggered: {
                    
                    // Close the sheet
                    fileViewerSheet.close();
                }
            }
        }
            
        // Create a container to hold all controls
        Container {
            
            // Arrange children top-to-bottom
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            
            // Create a list to hold browser results         
            ListView {
                
                // Set properties
                id: fileList
                dataModel : mainApp.fileListModel
                    
                // Define the list item template
                listItemComponents: [
                    
                    ListItemComponent {
                        
                        // Use a standard list item
                        StandardListItem {
                            
                            // Set properties
                            id: fileListItem
                            title: ListItemData.name
                            imageSource: ListItemData.thumbnailUrl
                            status: ListItemData.type
                            
                            // Per-item context actions
                            contextActions: [
                                
                                ActionSet {

                                    // Set properties
                                    title: ListItemData.name
                                    subtitle: ListItemData.type
                                    
                                    // Properties action item
                                    ActionItem {
                                        
                                        // Set properties
                                        title: "Properties"
                                        imageSource: "asset:///images/ic_properties.png"
                                        
                                        // Handler for trigger
                                        onTriggered: {

                                            // Call helper function to show file properties
                                            fileListItem.ListItem.view.showFileProperties(fileListItem.ListItem.indexPath);
                                        }
                                    }

                                    // Remove action item
                                    DeleteActionItem {

                                        // Set properties
                                        title: "Remove"

                                        // Handler for trigger
                                        onTriggered: {

                                            // Call helper function to remove file item
                                            fileListItem.ListItem.view.removeFiles(fileListItem.ListItem.indexPath);
                                        }
                                    }

                                    // Multiple selection action item
                                    MultiSelectActionItem {
                                        
                                        // Set properties
                                        multiSelectHandler: fileListItem.ListItem.view.multiSelectHandler

                                        // Handler for trigger
                                        onTriggered: {
                                            
                                            // Enable the multiple selection handler
                                            multiSelectHandler.active = true;
                                        }
                                    }
                                }
                            ]
                        }
                    }
                ]
                
                // Multiple selection handler
                multiSelectHandler {
                    
                    // Set default status
                    status: "None selected"
                    
                    // Multiple-item context actions
                    actions: [
                        
                        // Remove action item
                        DeleteActionItem {
                            
                            // Set properties
                            title: "Remove"
                            
                            // Handler for trigger
                            onTriggered: {
                                
                                // Remove the selected items
                                fileList.removeFiles(fileList.selectionList());
                                fileList.clearSelection();
                            }
                        }
                    ]
                }
                
                // Handler for trigger
                onTriggered: {
                    
                    // Invoke the file viewer
                    mainApp.viewFile(indexPath);
                }
                
                // Handler for selection change
                onSelectionChanged: {
                    
                    // Update multiple selection status
                    updateMultiStatus();
                }
                
                // Helper function to update multiple selection status
                function updateMultiStatus() {

                    // The status text of the multi-select handler is updated to show how
                    // many items are currently selected.
                    if (selectionList().length > 1) {
                        multiSelectHandler.status = selectionList().length + " items selected";
                    } else if (selectionList().length == 1) {
                        multiSelectHandler.status = "1 item selected";
                    } else {
                        multiSelectHandler.status = "None selected";
                    }
                }

                // Helper function to show file properties
                function showFileProperties(indexPath) {

                    // Show the file properties page
                    fileProperties.fileItem = dataModel.data(indexPath);
                    fileProperties.open();
                }
                
                // Helper function to remove file(s)
                function removeFiles(indexPath) {
                    
                    // Show confirmation dialog
                    if( indexPath.length > 0 ) {
                        confirmRemoveDialog.removeIndexPath = indexPath;
                        confirmRemoveDialog.body = "This will remove the selected item(s) from the slideshow."
                        confirmRemoveDialog.show();
                    }
                }
            }
        }
        
        // Page action items
        actions: [

            // Create to top action
            ActionItem {

                // Set properties
                id: toTopAction
                title: "To Top"
                imageSource: "asset:///images/ic_to_top.png"
                ActionBar.placement: ActionBarPlacement.InOverflow

                // Handler for trigger
                onTriggered: {

                    // Scroll to top
                    fileList.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.Default);
                }
            },

            // Create to bottom action
            ActionItem {

                // Set properties
                id: toBottomAction
                title: "To Bottom"
                imageSource: "asset:///images/ic_to_bottom.png"
                ActionBar.placement: ActionBarPlacement.InOverflow

                // Handler for trigger
                onTriggered: {

                    // Scroll to bottom
                    fileList.scrollToPosition(ScrollPosition.End, ScrollAnimation.Default);
                }
            },

            // Create add action
            ActionItem {
                
                // Set properties
                id: addAction
                title: "Add"
                imageSource: "asset:///images/ic_add.png"
                ActionBar.placement: ActionBarPlacement.Signature
                
                // Handler for trigger
                onTriggered: {
                    
                    // Clear selection and open the file picker
                    fileList.clearSelection();
                    filePicker.open();
                }
            },

            // Multiple selection action item
            MultiSelectActionItem {

                // Set properties
                multiSelectHandler: fileList.multiSelectHandler
                ActionBar.placement: ActionBarPlacement.InOverflow
                
                // Handler for trigger
                onTriggered: {

                    // Enable the multiple selection handler
                    multiSelectHandler.active = true;
                }
            }
        ]
        
        // Page attached objects
        attachedObjects: [
            
            // System dialog 
            SystemDialog {

                // Set properties
                id: confirmRemoveDialog
                title: "Remove?"
                
                // Create variable to hold selection list
                property variant removeIndexPath;
                
                // Handler for finished
                onFinished: {
                    
                    // Check user response
                    if( result == SystemUiResult.ConfirmButtonSelection ) {
                        // Remove file item(s)
                        mainApp.removeFiles(removeIndexPath);
                    }
                    
                    // Clear the selection list
                    removeIndexPath = null;
                }
            },
            
            // File picker definition
            FilePicker {
                
                // Set properties
                id: filePicker
                title : "Choose Pictures"
                type : FileType.Picture
                mode: FilePickerMode.PickerMultiple
                
                // Handler for file selected signal
                onFileSelected : {
                    
                    // Add items to file list
                    mainApp.addFileList(selectedFiles);
                }
            },
            
            // File properties definition
            FileProperties {
                
                // Set properties
                id: fileProperties
            }
        ]
    }
}
