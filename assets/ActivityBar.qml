import bb.cascades 1.0
import bb.system 1.0

Container {
    
    // Define property aliases
    property alias text: titleField.text
    
    // Arrange children using docking
    layout: DockLayout {
    }
    
    // Create empty container for translucent background
    Container {

        // Set properties
        background: Color.Black
        opacity: 0.5

        // Fill horizontally and vertically
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
    }
    
    // Create container for controls
    Container {
        
        // Arrange controls top-to-bottom
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        
        // Fill horizontally and vertically
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        // Create top divider
        Divider {
        }
        
        // Create container for controls
        Container {
            
            // Arrange controls left-to-right
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
            // Set padding / margins
            leftPadding: 20
            rightPadding: 20
            
            // Define the title field
            Label {
                
                // Set properties
                id: titleField
                textStyle {
                    base: SystemDefaults.TextStyles.PrimaryText
                    textAlign: TextAlign.Left
                }
                
                // Fill up space
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                
                // Fill vertically
                verticalAlignment: VerticalAlignment.Fill
            }
            
            // Define the activity indicator
            ActivityIndicator {
                
                // Set properties
                id: activityField
                running: true
                
                // Use required space
                layoutProperties: StackLayoutProperties {
                    spaceQuota: -1
                }
                
                // Fill vertically
                verticalAlignment: VerticalAlignment.Fill
            }
        }
        
        // Create bottom divider
        Divider {
        }
    }
}
