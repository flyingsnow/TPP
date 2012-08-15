enyo.kind({
	name: "TouchMupdf",
	kind: enyo.VFlexBox,
    published: {currentAngle: 1},
	components: [
        {kind: "SlidingPane", flex: 1, components: [
            {name: "left", width: "320px",components:[
                {kind: enyo.Control, name: "content1", sytle: "background-color: LightSkyBlue; padding: 0px 0px;",
                 content: "test content 1"},
                {kind: enyo.Control, name: "content2", sytle: "background-color: LightSkyBlue; padding: 0px 0px;", 
                content: "test content 2"},
            ]},
            {name: "right", flex: 1, components: [
                {kind: enyo.Control, name: "output", sytle: "background-color: LightSkyBlue; padding: 0px 0px;"},
                {kind: enyo.Hybrid, name: "mupdf", width: 1024, height: 768, params: ["/media/internal/downloads/a.pdf"],
                 executable: "touch_mupdf", alphablend: false, bgcolor: "404040", style: "padding: 0px 0px;", 
                 onPluginReady: "handlePluginReady"}
            ]}
        ]}
	],

    pluginReady: false,

    create: function() {
        this.inherited(arguments);
    },

    resizedChanged: function() {
        this.$.mupdf.resize();
    },
    currentAngleChanged: function() {
        this.$.output.setContent("AngleChanged: "+this.currentAngle);
    },
/*
    gesturestarHandler: function(inSender, event) {
       enyo.stopEvent(event);
    },
    gestureendHandler: function(inSender, event) {

       enyo.stopEvent(event);
    },
*/
    gesturechangeHandler: function(inSender, event) {
       enyo.stopEvent(event);
        this.setCurrentAngle(event.scale);
    },

    handlePluginReady: function() {
        this.$.output.setContent("Plugin initalized");
        this.pluginReady = true;
    },


    mousedownHandler: function(inSender, event) {
        if(this.pluginReady) {
            try{
             var status = this.$.mupdf.callPluginMethod("flipPage", ",");
            }
            catch (e) {
                this.$.output.setContent("plugin excepton: " + e);
            }
        }
        else {
            this.$.output.setContent("plugin not ready");
        }
        this.$.output.setContent("mouse down ...");
    },
 
    mousemoveHandler: function(inSender, event) {
              this.$.output.setContent("move page...");
    },

    mouseupHandler: function(inSender, event) {
        if(this.pluginReady) {
            try{
             var status = this.$.mupdf.callPluginMethod("flipPage", ".");
              this.$.output.setContent("flip page...");
            }
            catch (e) {
                this.$.output.setContent("plugin excepton: " + e);
            }
        }
        else {
            this.$.output.setContent("plugin not ready");
        }
    },

/*
    handleMouseup: function() {
        if(this.pluginReady) {
            try{
             var status = this.$.mupdf.callPluginMethod("flipPage", "+");
              this.$.output.setContent("flip page...");
            }
            catch (e) {
                this.$.output.setContent("plugin excepton: " + e);
            }
        }
        else {
            this.$.output.setContent("plugin not ready");
        }
    }
*/
});

