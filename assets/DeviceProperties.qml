import bb.cascades 1.0

// Use a Sheet to hold contents
Sheet {

    // Define properties
    property variant deviceViewer
    
    // Set properties
    id: devicePropertiesSheet
    peekEnabled: false
    
    // Create the main page
    Page {
        
        // Set properties
        id: devicePropertiesPage

        // Create title bar
        titleBar: TitleBar {
            
            // Set properties
            title: "Properties"

            // Create done action
            acceptAction: ActionItem {

                // Set properties
                title: "Done"

                // Handler for trigger
                onTriggered: {

                    // Close the sheet
                    devicePropertiesSheet.close();
                }
            }
        }
        
        // Create a container to hold all controls
        Container {
            
            // Arrange children top-to-bottom
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            
            // Set some padding for child controls
            topPadding: 10
            bottomPadding: 10
            leftPadding: 10
            rightPadding: 10
            
            // Fill horizontally
            horizontalAlignment: HorizontalAlignment.Fill
            
            // Create the name field
            ViewerField {

                // Set properties
                title: "Name:"
                value: deviceViewer.device.name

                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }
            
            // Create the address field
            ViewerField {
                
                // Set properties
                title: "Address:"
                value: deviceViewer.device.address
                
                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }
            
            // Create the port field
            ViewerField {
                
                // Set properties
                title: "Port:"
                value: deviceViewer.device.port
                
                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }
            
            // Create the device ID field
            ViewerField {
                
                // Set properties
                title: "Device ID:"
                value: deviceViewer.device.deviceID
                
                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }
            
            // Create the model field
            ViewerField {
                
                // Set properties
                title: "Model:"
                value: deviceViewer.device.model
                
                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }
            
            // Create the password required field
            ViewerField {
                
                // Set properties
                title: "Password Required:"
                value: deviceViewer.device.passwordRequired
                
                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }
        }
    }
}
