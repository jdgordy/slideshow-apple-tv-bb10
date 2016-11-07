import bb.cascades 1.0

// Use a Sheet to hold contents
Sheet {

    // Define property aliases
    property variant fileItem

    // Set properties
    id: filePropertiesSheet
    peekEnabled: false
    
    // Create the main page
    Page {
        
        // Set properties
        id: filePropertiesPage

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
                    filePropertiesSheet.close();
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
            
            // Create the picture preview
            ImageView {
                
                // Set properties
                id: imageView
                imageSource: filePropertiesSheet.fileItem.url
                scalingMethod: ScalingMethod.AspectFit
                loadEffect: ImageViewLoadEffect.Subtle

                // Use remaining space
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                
                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }
            
            // Create the name field
            ViewerField {
                
                // Set properties
                title: "Name:"
                value: filePropertiesSheet.fileItem.name
                
                // Use required space
                layoutProperties: StackLayoutProperties {
                    spaceQuota: -1
                }

                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }

            // Create the name field
            ViewerField {

                // Set properties
                title: "Location:"
                value: filePropertiesSheet.fileItem.location
                
                // Use required space
                layoutProperties: StackLayoutProperties {
                    spaceQuota: -1
                }

                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }
        }
    }
}
