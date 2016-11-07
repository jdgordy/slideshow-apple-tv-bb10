import bb.cascades 1.0
import bb.system 1.0
import my.library 1.0

// Use a Sheet to hold contents
Sheet {

    // Set properties
    id: deviceViewerSheet
    peekEnabled: false

    // Handler for opened signal
    onOpened: {
        
        // Start timer if no records found
        if( mainApp.recordListModel.size() == 0 ) {
            // Start timer for ten seconds
            deviceTimer.start(10000);
        }
    }
    
    // Create the main page
    Page {
        
        // Set properties
        id: deviceViewerPage
        
        // Create title bar
        titleBar: TitleBar {
            
            // Set properties
            title: "Choose Device"
            
            // Create cancel action
            dismissAction: ActionItem {
                
                // Set properties
                title: "Cancel"
                
                // Handler for trigger
                onTriggered: {
                    
                    // Stop timer
                    deviceTimer.stop();
                    
                    // Close the sheet
                    deviceViewerSheet.close();
                }
            }
        }
        
        // Create a container to hold controls and activity indicator overlay
        Container {
            
            // Arrange children using docking
            layout: DockLayout {
            }
            
            // Create a container to hold device list
            Container {
                
                // Arrange children top-to-bottom
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }

                // Fill horizontally and vertically
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                
                // Create a list to hold browser results         
                ListView {
                    
                    // Set properties
                    id: recordList
                    dataModel : mainApp.recordListModel
                    
                    // Define the list item template
                    listItemComponents: [
                        
                        ListItemComponent {
                            
                            // Use a standard list item
                            StandardListItem {
                                
                                // Set properties
                                id: rootItem
                                title: ListItemData.name
                                status: ListItemData.serviceType
                                
                                // Context menu actions
                                contextActions: [
                                    
                                    ActionSet {
                                        
                                        // Set properties
                                        title: ListItemData.name
                                        subtitle: "AirPlay Device"
                                        
                                        // Create properties action                                        
                                        ActionItem {
                                            
                                            // Set properties
                                            title: "Properties"
                                            imageSource: "asset:///images/ic_properties.png"
                                            
                                            // Handler for trigger
                                            onTriggered: {
                                                
                                                // Call helper function to show device properties
                                                rootItem.ListItem.view.showDeviceProperties(rootItem.ListItem.indexPath);
                                            }
                                        }
                                    }
                                ]
                            }
                        }
                    ]
                    
                    // Helper function to show device properties
                    function showDeviceProperties(indexPath) {
                        
                        // View device properties
                        mainApp.viewDevice(indexPath);
                        
                        // Show the device properties page
                        deviceProperties.open();
                    }
                    
                    // Handler for trigger            
                    onTriggered: {
                        
                        // Set current device and close the sheet
                        mainApp.setDevice(indexPath);
                        deviceViewerSheet.close();
                    }
                    
                    attachedObjects: [
                        
                        // Device properties viewer
                        DeviceProperties {
                            
                            // Set properties
                            id: deviceProperties
                            deviceViewer: mainApp.devicePropertiesViewer
                        }
                    ]
                }
            }
            
            // Create activity indicator for empty list
            ActivityBar {
                
                // Set properties
                text: "Searching for AirPlay devices..."
                
                // Visible only if no devices are found
                visible: mainApp.recordListModel.size() == 0
                
                // Fill horizontally and align at the top
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Top
            }
        }
        
        attachedObjects: [
            
            // System toast
            SystemToast {
                
                // Set properties
                id: deviceSystemToast
                body: ""
            },
            
            // Timer
            QTimer {
                
                // Set properties
                id: deviceTimer
                singleShot: true
                
                // Handler for timeout
                onTimeout: {
                    
                    // Display error toast
                    deviceSystemToast.body = "No devices found yet.  Check that your Apple TV is connected to the local network and that AirPlay is enabled.";
                    deviceSystemToast.show();
                }
            }
        ]
    }
}
