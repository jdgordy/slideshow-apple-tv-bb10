import bb.cascades 1.0

// Help page
Page {

    // Set properties
    id: helpPage
    titleBar: TitleBar {
        title: "Help"
    }

    // Create a scroll view to hold web view
    ScrollView {
        
        // Set properties
        scrollViewProperties {
            scrollMode: ScrollMode.Vertical
            overScrollEffectMode: OverScrollEffectMode.None
            pinchToZoomEnabled: false
        }
        
        // Create a container to hold web view
        Container {
            
            // Set properties
            background: Color.Black
            
            // Arrange children top-to-bottom
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }

            // Fill vertically and vertically
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill

            // Create the web view
            WebView {

                // Set properties
                id: webView
                settings.devicePixelRatio: 1.0
                settings.defaultFontSizeFollowsSystemFontSize: true

                // Set content
                url: "local:///assets/web/help.html"
            }
        }
    }
}
