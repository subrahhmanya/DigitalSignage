(function (window) {
    'use strict';

    // Define all variables
    var iBoxes = [], selectionHandles = [], canvas, ctx, WIDTH, HEIGHT, INTERVAL = 20, CPINTERVAL = 20, cPhase = 0, doFormUpdate = false, isDrag = false, isResizeDrag = false, expectResize = -1, mx, my, canvasValid = false, mySel = null, mySelColor = '#CC0000', mySelWidth = 4, mySelBoxColor = 'darkred', mySelBoxSize = 10, ghostcanvas, gctx, offsetx, offsety, stylePaddingLeft, stylePaddingTop, styleBorderLeft, styleBorderTop;

    // Box object to hold data
    function Box2() {
        this.x = 0;
        this.y = 0;
        this.w = 1; // default width and height?
        this.h = 1;
        this.uid = 0;
        this.scale = 255;
        this.fill = '#FFFFFF';
        this.ox = 0;
        this.oy = 0;
        this.ow = 1; // default width and height?
        this.oh = 1;
        this.ouid = 0;
        this.oscale = 255;
        this.ofill = '#FFFFFF';
    }

    function updateGradients() {
        if (mySel !== null) {
            cPhase = cPhase + 0.1;
            if (cPhase > 32.0) {
                cPhase = 0;
            }
        }
    }

    function cutHex(h) {return (h.charAt(0)=="#") ? h.substring(1,7):h}
    function hexToR(h) {return parseInt((cutHex(h)).substring(0,2),16)}
    function hexToG(h) {return parseInt((cutHex(h)).substring(2,4),16)}
    function hexToB(h) {return parseInt((cutHex(h)).substring(4,6),16)}

    function toHex(n) {
        n = parseInt(n, 10);
        if (isNaN(n)) {
            return "00";
        }
        n = Math.max(0, Math.min(n, 255));
        return "0123456789ABCDEF".charAt((n - n % 16) / 16) + "0123456789ABCDEF".charAt(n % 16);
    }

    function rgbToHex(R, G, B) {
        return toHex(R) + toHex(G) + toHex(B);
    }

    function cGradientVal(tConc, tGrads, cycle, phase) {
        var center, width, frequency, red, green, blue;

        if (phase === undefined) {
            phase = 0;
        }

        center = 128;
        width = 127;
        frequency = Math.PI * 2 / tConc;
        red = Math.round(Math.sin(frequency * cycle + 2 + phase) * width + center);
        green = Math.round(Math.sin(frequency * cycle + phase) * width + center);
        blue = Math.round(Math.sin(frequency * cycle + 4 + phase) * width + center);
        return (rgbToHex(red, green, blue));
    }

    //wipes the canvas context
    function clear(c) {
        c.clearRect(0, 0, WIDTH, HEIGHT);
    }

    function invalidate() {
        canvasValid = false;
    }

    // New methods on the Box class
    Box2.prototype = {
        // we used to have a solo draw function
        // but now each box is responsible for its own drawing
        // mainDraw() will call this with the normal canvas
        // myDown will call this with the ghost canvas with 'black'
        drawSelBox: function (context, optionalColor) {
            // draw selection
            // this is a stroke along the box and also 8 new selection handles
            if (mySel === this) {
                // Update Variables on Form //
                if (this.x !== this.ox || this.y !== this.oy || this.w !== this.ow || this.h !== this.oh || this.scale !== this.oscale || this.uid !== this.ouid || this.fill !== this.ofill) {
                    this.ox = this.x;
                    this.oy = this.y;
                    this.ow = this.w;
                    this.oh = this.h;
                    this.oscale = this.scale;
                    this.ouid = this.uid;
                    this.ofill = this.fill;
                    doFormUpdate = true;
                }

                if (doFormUpdate === true) {
                    document.forms.iBoxDetails.xcoord.value = Math.round(this.x);
                    document.forms.iBoxDetails.ycoord.value = Math.round(HEIGHT - (this.y + this.h));
                    document.forms.iBoxDetails.width.value = Math.round(this.w);
                    document.forms.iBoxDetails.height.value = Math.round(this.h);
                    document.forms.iBoxDetails.scale.value = this.scale;
                    document.forms.iBoxDetails.boxid.value = this.uid;
                    document.forms.iBoxDetails.boxcol.value = this.fill;
                    doFormUpdate = false;
                }


                //context.strokeStyle = mySelColor;
                var grad = ctx.createLinearGradient(0, 0, 1280, 720), i, half, cur, cGrad;
                for (i = 0; i < 64; i += 1) {
                    cGrad = '#' + cGradientVal(32, 64, i, cPhase);
                    grad.addColorStop(i / 64, cGrad);
                }

                context.strokeStyle = grad;
                context.lineWidth = mySelWidth;
                context.strokeRect(this.x, this.y + (this.h - ((this.h / 255) * this.scale)), ((this.w / 255) * this.scale), ((this.h / 255) * this.scale));

                // draw the boxes

                half = mySelBoxSize / 2;

                // 0  1  2
                // 3     4
                // 5  6  7

                // top left, middle, right
                selectionHandles[0].x = this.x - half;
                selectionHandles[0].y = (this.y + (this.h - ((this.h / 255) * this.scale))) - half;

                selectionHandles[1].x = this.x + ((this.w / 255) * this.scale) / 2 - half;
                selectionHandles[1].y = (this.y + (this.h - ((this.h / 255) * this.scale))) - half;

                selectionHandles[2].x = this.x + ((this.w / 255) * this.scale) - half;
                selectionHandles[2].y = (this.y + (this.h - ((this.h / 255) * this.scale))) - half;

                //middle left
                selectionHandles[3].x = this.x - half;
                selectionHandles[3].y = (this.y + (this.h - ((this.h / 255) * this.scale))) + ((this.h / 255) * this.scale) / 2 - half;

                //middle right
                selectionHandles[4].x = this.x + ((this.w / 255) * this.scale) - half;
                selectionHandles[4].y = (this.y + (this.h - ((this.h / 255) * this.scale))) + ((this.h / 255) * this.scale) / 2 - half;

                //bottom left, middle, right
                selectionHandles[6].x = this.x + ((this.w / 255) * this.scale) / 2 - half;
                selectionHandles[6].y = (this.y + (this.h - ((this.h / 255) * this.scale))) + ((this.h / 255) * this.scale) - half;

                selectionHandles[5].x = this.x - half;
                selectionHandles[5].y = (this.y + (this.h - ((this.h / 255) * this.scale))) + ((this.h / 255) * this.scale) - half;

                selectionHandles[7].x = this.x + ((this.w / 255) * this.scale) - half;
                selectionHandles[7].y = (this.y + (this.h - ((this.h / 255) * this.scale))) + ((this.h / 255) * this.scale) - half;


                context.fillStyle = grad;
                for (i = 0; i < 8; i += 1) {
                    cur = selectionHandles[i];
                    context.fillRect(cur.x, cur.y, mySelBoxSize, mySelBoxSize);
                }
            }
        },

        draw: function (context, optionalColor) {
            var tAlpha, tR, tB, tG;
            tR = hexToR(this.fill);
            tG = hexToG(this.fill);
            tB = hexToB(this.fill);
            if (context === gctx) {
                context.fillStyle = 'black'; // always want black for the ghost canvas
            } else {
                if (mySel !== null) {
                    if (mySel !== this) {
                        tR = tR - 75;
                        tG = tG - 75;
                        tB = tB - 75;
                        if (tR < 0) {
                            tR = 0;
                        }
                        if (tG < 0) {
                            tG = 0;
                        }
                        if (tB < 0) {
                            tB = 0;
                        }
                    }
                }
                tAlpha = 'rgba(' + tR + ',' + tG + ',' + tB + ',1.0)';
                context.fillStyle = tAlpha;
            }

            // We can skip the drawing of elements that have moved off the screen:
            if (this.x > WIDTH || this.y > HEIGHT) {
                return;
            }
            if (this.x + ((this.w / 255) * this.scale) < 0 || this.y + ((this.h / 255) * this.scale) < 0) {
                return;
            }

            context.fillRect(this.x, (this.y + (this.h - ((this.h / 255) * this.scale))), ((this.w / 255) * this.scale), (this.h / 255) * this.scale);
        } // end draw
    };

    //Select Next Box
    function selNextRect() {
        doFormUpdate = true;
        var tNB = 0, i;
        if (mySel === null) {
            mySel = iBoxes[0];
            offsetx = mx - mySel.x;
            offsety = my - mySel.y;
            mySel.x = mx - offsetx;
            mySel.y = my - offsety;
            isDrag = false;

            invalidate();
            clear(gctx);
        } else {
            for (i = 0; i < iBoxes.length; i += 1) {
                if (iBoxes[i] === mySel) {
                    if (i < (iBoxes.length - 1)) {
                        tNB = i + 1;
                    } else {
                        tNB = 0;
                    }

                    mySel = iBoxes[tNB];

                    offsetx = mx - mySel.x;
                    offsety = my - mySel.y;
                    mySel.x = mx - offsetx;
                    mySel.y = my - offsety;
                    isDrag = false;

                    invalidate();
                    clear(gctx);
                    break;
                }
            }
        }
    }

    //Initialize a new Box, add it, and invalidate the canvas
    function addRect(x, y, w, h, fill, scale, bUID) {
        var rect = new Box2();
        rect.x = parseInt(x, 10);
        rect.y = parseInt(y, 10);
        rect.w = parseInt(w, 10);
        rect.h = parseInt(h, 10);
        if (bUID === undefined) {
            rect.uid = Math.round((new Date()).getTime());
        } else {
            rect.uid = bUID;
        }
        rect.scale = scale;
        rect.fill = fill;
        iBoxes.push(rect);

        // Clear Form Data on Create.
        document.forms.iBoxDetails.xcoord.value = "";
        document.forms.iBoxDetails.ycoord.value = "";
        document.forms.iBoxDetails.width.value = "";
        document.forms.iBoxDetails.height.value = "";
        document.forms.iBoxDetails.scale.value = "";
        document.forms.iBoxDetails.boxid.value = "";
        document.forms.iBoxDetails.boxcol.value = "";

        invalidate();
    }

    //Modify Box
    function modRect(bID, x, y, w, h, fill, s) {
        var i;
        for (i = 0; i < iBoxes.length; i += 1) {
            if (iBoxes[i].uid === parseInt(bID, 10)) {
                iBoxes[i].x = parseInt(x, 10);
                iBoxes[i].y = parseInt(y, 10);
                iBoxes[i].w = parseInt(w, 10);
                iBoxes[i].h = parseInt(h, 10);
                iBoxes[i].scale = parseInt(s, 10);
                iBoxes[i].fill = fill;
                doFormUpdate = true;
            }
        }
        invalidate();
    }

    //Remove Box when using destroyer event
    function delRect(bID) {
        var i;
        for (i = 0; i < iBoxes.length; i += 1) {
            if (iBoxes[i].uid === parseInt(bID, 10)) {
                iBoxes.splice(i, 1);
                // Need to select nothing.
                mySel = null;
                // Nothing Selected, clear variables on form.
                document.forms.iBoxDetails.xcoord.value = "";
                document.forms.iBoxDetails.ycoord.value = "";
                document.forms.iBoxDetails.width.value = "";
                document.forms.iBoxDetails.height.value = "";
                document.forms.iBoxDetails.scale.value = "";
                document.forms.iBoxDetails.boxid.value = "";
                document.forms.iBoxDetails.boxcol.value = "";
                break;
            }
        }
        invalidate();
    }

    // Main draw loop.
    // While draw is called as often as the INTERVAL variable demands,
    // It only ever does something if the canvas gets invalidated by our code
    function mainDraw() {
        if (mySel !== null) {
            // Animate Selection Border if selected, so always invalidate.
            invalidate();
        }

        if (canvasValid === false) {
            clear(ctx);

            // Add stuff you want drawn in the background all the time here

            // draw all boxes
            var l = iBoxes.length, i;
            for (i = 0; i < l; i += 1) {
                iBoxes[i].draw(ctx); // we used to call drawshape, but now each box draws itself
            }

            // Now draw Selection Boxes (if any)
            for (i = 0; i < l; i += 1) {
                iBoxes[i].drawSelBox(ctx); // we used to call drawshape, but now each box draws itself
            }

            // Add stuff you want drawn on top all the time here

            canvasValid = true;
        }
    }

    // Sets mx,my to the mouse position relative to the canvas
    // unfortunately this can be tricky, we have to worry about padding and borders
    function getMouse(e) {
        var element = canvas,
            offsetX = 0,
            offsetY = 0;

        if (element.offsetParent) {
            do {
                offsetX += element.offsetLeft;
                offsetY += element.offsetTop;
            } while ((element = element.offsetParent));
        }

        // Add padding and border style widths to offset
        offsetX += stylePaddingLeft;
        offsetY += stylePaddingTop;

        offsetX += styleBorderLeft;
        offsetY += styleBorderTop;

        mx = (e.pageX - offsetX) * ((1 / 800) * 1280);
        my = (e.pageY - offsetY) * ((1 / 450) * 720);

        document.forms.iBoxDetails.mousex.value = Math.round(mx);
        document.forms.iBoxDetails.mousey.value = Math.round(my);
    }

    // Happens when the mouse is clicked in the canvas
    function myDown(e) {
        getMouse(e);

        //we are over a selection box
        if (expectResize !== -1) {
            isResizeDrag = true;
            return;
        }

        clear(gctx);
        var l = iBoxes.length, i, imageData, index;
        for (i = l - 1; i >= 0; i -= 1) {
            // draw shape onto ghost context
            iBoxes[i].draw(gctx, 'black');

            // get image data at the mouse x,y pixel
            imageData = gctx.getImageData(mx, my, 1, 1);
            index = (mx + my * imageData.width) * 4;

            // if the mouse pixel exists, select and break
            if (imageData.data[3] > 0) {
                mySel = iBoxes[i];
                offsetx = mx - mySel.x;
                offsety = my - mySel.y;
                mySel.x = mx - offsetx;
                mySel.y = my - offsety;
                isDrag = true;
                doFormUpdate = true;

                invalidate();
                clear(gctx);
                return;
            }

        }
        // havent returned means we have selected nothing
        mySel = null;
        // Nothing Selected, clear variables on form.
        document.forms.iBoxDetails.xcoord.value = "";
        document.forms.iBoxDetails.ycoord.value = "";
        document.forms.iBoxDetails.width.value = "";
        document.forms.iBoxDetails.height.value = "";
        document.forms.iBoxDetails.scale.value = "";
        document.forms.iBoxDetails.boxid.value = "";
        document.forms.iBoxDetails.boxcol.value = "";

        // clear the ghost canvas for next time
        clear(gctx);
        // invalidate because we might need the selection border to disappear
        invalidate();
    }

    function myUp() {
        isDrag = false;
        isResizeDrag = false;
        expectResize = -1;
    }

    // adds a new node
    function myDblClick(e) {
        getMouse(e);
        // Create a standard box, 50x50px in size, centered at the current mouse pos.
        var width = 50, height = 50;
        addRect(mx - (width / 2), my - (width / 2), width, height, '#FFFFFF', 255);
    }

    // Happens when the mouse is moving inside the canvas
    function myMove(e) {
        var i, oldx, oldy, cur;
        if (isDrag) {
            getMouse(e);
            doFormUpdate = true;

            mySel.x = mx - offsetx;
            mySel.y = my - offsety;

            // something is changing position so we better invalidate the canvas!
            invalidate();
        } else if (isResizeDrag) {
            // time ro resize!
            oldx = mySel.x;
            oldy = mySel.y;

            // 0  1  2
            // 3     4
            // 5  6  7
            switch (expectResize) {
            case 0:
                mySel.x = mx;
                mySel.y = my;
                mySel.w += oldx - mx;
                mySel.h += oldy - my;
                break;
            case 1:
                mySel.y = my;
                mySel.h += oldy - my;
                break;
            case 2:
                mySel.y = my;
                mySel.w = (mx + (mySel.w - ((mySel.w / 255) * mySel.scale))) - oldx;
                mySel.h += oldy - my;
                break;
            case 3:
                mySel.x = mx;
                mySel.w += oldx - mx;
                break;
            case 4:
                mySel.w = (mx + (mySel.w - ((mySel.w / 255) * mySel.scale))) - oldx;
                break;
            case 5:
                mySel.x = mx;
                mySel.w += oldx - mx;
                mySel.h = my - oldy;
                break;
            case 6:
                mySel.h = my - oldy;
                break;
            case 7:
                mySel.w = (mx + (mySel.w - ((mySel.w / 255) * mySel.scale))) - oldx;
                mySel.h = my - oldy;
                break;
            }

            //Ensure we don't go into negatives
            if (mySel.w < 1) {
                mySel.w = 1;
            }
            if (mySel.h < 1) {
                mySel.h = 1;
            }
            invalidate();
        }

        getMouse(e);
        // if there's a selection see if we grabbed one of the selection handles
        if (mySel !== null && !isResizeDrag) {
            for (i = 0; i < 8; i += 1) {
                // 0  1  2
                // 3     4
                // 5  6  7

                cur = selectionHandles[i];

                // we dont need to use the ghost context because
                // selection handles will always be rectangles
                if (mx >= cur.x && mx <= cur.x + mySelBoxSize && my >= cur.y && my <= cur.y + mySelBoxSize) {
                    // we found one!
                    expectResize = i;
                    invalidate();

                    switch (i) {
                    case 0:
                        this.style.cursor = 'nw-resize';
                        break;
                    case 1:
                        this.style.cursor = 'n-resize';
                        break;
                    case 2:
                        this.style.cursor = 'ne-resize';
                        break;
                    case 3:
                        this.style.cursor = 'w-resize';
                        break;
                    case 4:
                        this.style.cursor = 'e-resize';
                        break;
                    case 5:
                        this.style.cursor = 'sw-resize';
                        break;
                    case 6:
                        this.style.cursor = 's-resize';
                        break;
                    case 7:
                        this.style.cursor = 'se-resize';
                        break;
                    }
                    return;
                }

            }
            // not over a selection box, return to normal
            isResizeDrag = false;
            expectResize = -1;
            this.style.cursor = 'auto';
        }
    }

    // initialize our canvas, add a ghost canvas, set draw loop
    // then add everything we want to intially exist on the canvas
    function init2() {
        canvas = document.getElementById('SCanvas');
        HEIGHT = canvas.height;
        WIDTH = canvas.width;
        ctx = canvas.getContext('2d');
        ghostcanvas = document.createElement('canvas');
        ghostcanvas.height = HEIGHT;
        ghostcanvas.width = WIDTH;
        gctx = ghostcanvas.getContext('2d');

        //fixes a problem where double clicking causes text to get selected on the canvas
        canvas.onselectstart = function () {
            return false;
        };

        // fixes mouse co-ordinate problems when there's a border or padding
        // see getMouse for more detail
        if (document.defaultView && document.defaultView.getComputedStyle) {
            stylePaddingLeft = parseInt(document.defaultView.getComputedStyle(canvas, null).paddingLeft, 10) || 0;
            stylePaddingTop = parseInt(document.defaultView.getComputedStyle(canvas, null).paddingTop, 10) || 0;
            styleBorderLeft = parseInt(document.defaultView.getComputedStyle(canvas, null).borderLeftWidth, 10) || 0;
            styleBorderTop = parseInt(document.defaultView.getComputedStyle(canvas, null).borderTopWidth, 10) || 0;
        }

        // make mainDraw() fire every INTERVAL milliseconds
        setInterval(mainDraw, INTERVAL);

        // set our events. Up and down are for dragging,
        // double click is for making new boxes
        canvas.onmousedown = myDown;
        canvas.onmouseup = myUp;
        canvas.ondblclick = myDblClick;
        canvas.onmousemove = myMove;

        // set up the selection handle boxes
        var i, rect;
        for (i = 0; i < 8; i += 1) {
            rect = new Box2();
            selectionHandles.push(rect);
        }

        // add custom initialization here:
        setInterval(updateGradients, CPINTERVAL);

    }

    // If you dont want to use <body onLoad='init()'>
    // You could uncomment this init() reference and place the script reference inside the body tag
    //init();
    window.init2 = init2;
    window.addRect = addRect;
    window.modRect = modRect;
    window.delRect = delRect;
    window.selNextRect = selNextRect;
}(window));
