import bb.cascades 1.0

// Info page
Page {

    // Set properties
    id: infoPage
    titleBar: TitleBar {
        title: "Info"
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
                url: "local:///assets/web/info.html"
            }
        }
    }
    
    actions: [

        // BlackBerry World action
        InvokeActionItem {

            // Set properties
            title: "Review"
            imageSource: "asset:///images/ic_app_world.png"
            ActionBar.placement: ActionBarPlacement.OnBar

            // Set invocation query
            query {
                invokeTargetId: "sys.appworld"
                invokeActionId: "bb.action.OPEN"
                uri: "appworld://content/32301888"
            }
        },
        
        // Privacy policy link
        InvokeActionItem {
            
            // Set properties
            title: "Privacy Policy"
            imageSource: "asset:///images/ic_legal.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            
            // Set invocation query
            query {
                invokeTargetId: "sys.browser"
                invokeActionId: "bb.action.OPEN"
                uri: "https://dl.dropboxusercontent.com/u/12102880/privacy.html"
            }
        }
    ]
}
