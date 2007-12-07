var windowsInternetExplorer = false;
function detectBrowser()
{
    windowsInternetExplorer = false;
    var appVersion = navigator.appVersion;
    if ((appVersion.indexOf("MSIE") != -1) &&
        (appVersion.indexOf("Macintosh") == -1))
    {
        windowsInternetExplorer = true;
    }
}

function fixupIEPNGBG(strImageID) 
{
    if (windowsInternetExplorer)
    {
        var oBlock = document.getElementById(strImageID);

        if (oBlock)
        {
            var currentBGStyle = oBlock.style.background;
            var urlStart = currentBGStyle.indexOf("url(");
            var urlEnd = currentBGStyle.indexOf(")", urlStart);
            var imageURL = currentBGStyle.substring(urlStart + 4, urlEnd);
            var filterStyle =
                "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" +
                imageURL +
                "', sizingMethod='crop');";
            oBlock.style.background = "";

            var backgroundImage = new Image();
            backgroundImage.src = imageURL;
            var tileWidth = backgroundImage.width;
            var tileHeight = backgroundImage.height; 
            
            if ((tileWidth == 0) || (tileHeight == 0))
            {
                // There seems to be something wrong with the image, return directly
                return;
            }

            var blockWidth = parseInt(oBlock.style.width);
            var blockHeight = parseInt(oBlock.style.height);
            var wholeCols = Math.floor(blockWidth / tileWidth);
            var wholeRows = Math.floor(blockHeight / tileHeight);
            var extraWidth = blockWidth - (tileWidth * wholeCols);
            var extraHeight = blockHeight - (tileHeight * wholeRows);
            
            if (currentBGStyle.indexOf("no-repeat") != -1)
            {
                // The style is no-repeat, so we only place one row and column of images
                wholeCols = 1;
                wholeRows = 1;
                extraWidth = 0;
                extraHeight = 0;
            }
            else if (currentBGStyle.indexOf("repeat-x") != -1)
            {
                // the style is repeat-x, so we only tile a single row
                wholeRows = 1;
                extraHeight = 0;
            }
            else if (currentBGStyle.indexOf("repeat-y") != -1)
            {
                // the style is repeat-y, so we only tile a single column
                wholeCols = 1;
                extraWidth = 0;
            }

            var newMarkup = "";
            
            for (var currentRow = 0; 
                 currentRow < wholeRows; 
                 currentRow++)
            {
                for (currentCol = 0; 
                     currentCol < wholeCols; 
                     currentCol++)
                {
                    newMarkup += "<div style=" +
                            "\"position: absolute; line-height: 0px; " +
                            "width: " + tileWidth + "px; " +
                            "height: " + tileHeight + "px; " +
                            "left:" + currentCol *  tileWidth + "px; " +
                            "top:" + currentRow *  tileHeight + "px; " +
                            "filter:" + filterStyle + 
                            "\" > </div>";
                }
                
                if (extraWidth != 0)
                {
                    // Add any extra bit required to fill out the row
                    newMarkup += "<div style=" +
                            "\"position: absolute; line-height: 0px; " +
                            "width: " + extraWidth + "px; " +
                            "height: " + tileHeight + "px; " +
                            "left:" + currentCol *  tileWidth + "px; " +
                            "top:" + currentRow *  tileHeight + "px; " +
                            "filter:" + filterStyle + 
                            "\" > </div>";
                }
            }
            
            if (extraHeight != 0)
            {
                // Add a final row with any extra bits required
                for (currentCol = 0; 
                     currentCol < wholeCols; 
                     currentCol++)
                {
                    newMarkup += "<div style=" +
                            "\"position: absolute; line-height: 0px; " +
                            "width: " + tileWidth + "px; " +
                            "height: " + extraHeight + "px; " +
                            "left:" + currentCol *  tileWidth + "px; " +
                            "top:" + currentRow *  tileHeight + "px; " +
                            "filter:" + filterStyle + 
                            "\" > </div>";
                }
                
                if (extraWidth != 0)
                {
                    // Add any extra bit required to fill out the row
                    newMarkup += "<div style=" +
                            "\"position: absolute; line-height: 0px; " +
                            "width: " + extraWidth + "px; " +
                            "height: " + extraHeight + "px; " +
                            "left:" + currentCol *  tileWidth + "px; " +
                            "top:" + currentRow *  tileHeight + "px; " +
                            "filter:" + filterStyle + 
                            "\" > </div>";
                }
            }
            
            // wrap the old content in an inner div to force it on top of the tiled images
            newMarkup += "<div style=" +
                            "\"position: absolute; line-height: 0px; " +
                            "z-index: 1;" +
                            "width: " + blockWidth + "px; " +
                            "height: " + blockHeight + "px; " +
                            "left: 0px; " +
                            "top: 0px; " +
                            "\" >";
            
            oBlock.innerHTML = newMarkup + oBlock.innerHTML + "</div>";
        }
    }
}

function onPageLoad()
{
    detectBrowser();
    fixupIEPNGBG("id1");
    fixupIEPNGBG("id2");
    fixupIEPNGBG("id3");
    fixupIEPNGBG("id4");
    return true;
}

