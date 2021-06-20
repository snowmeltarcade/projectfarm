# Styling
We use a local form of CSS to style UI controls.

Global CSS sets the default styles. You can override these styles by creating custom CSS classes and assigning them to UI control types or individual UI controls.

Each UI control state has its own CSS class. The UI control uses the CSS class assigned to the active state when rendering.

### UI Control States
The UI control states that have associated CSS classes are:

* **Normal**
  When no other states are active. This is the default Ui state.
* **Disabled**
  When the internal `_isEnabled` flag is `false`
* **Mouse Over**
  When the mouse cursor is over the control.
* **Mouse Down**
  When clicking or tapping the control 
* **Focused**
  When this control has active focus.
  
If multiple states are active at the same time, the UI system will use the following order of precedence to decide which style to apply:

1. Disabled
1. Mouse Down
1. Mouse Over
1. Focused
1. Normal

### CSS Styling
We will adhere closely to the CSS syntax defined here: https://developer.mozilla.org/en-US/docs/Web/CSS/Syntax

### Attributes
The following attributes can be used to style UI controls. Please note that not all controls may use each of these styles.

* **color**
  Defines the color in RGB. An alias for `color-rgb`. Please see the [colors](../Graphics/Colors.md) documentation for valid color values
* **color-rgb**
  Defines the color in RGB. If `color-hsv` is also set, `color-rgb` will override this value. Please see the [colors](../Graphics/Colors.md) documentation for valid color values
* **color-hsv**
  Defines the color in HSV. If `color-rgb` is also set, that will override this value. Please see the [colors](../Graphics/Colors.md) documentation for valid color values
* **hue**
  Defines the hue of the set color. This will override the `h` value in `color-hsv` or will modify the color set in `color-rgb`. This value must be in the `0..1` range
* **saturation**
  Defines the saturation of the set color. This will override the `s` value in `color-hsv` or will modify the color set in `color-rgb`. This value must be in the `0..1` range
* **brightness**
  Defines the brightness of the set color. This will override the `v` value in `color-hsv` or will modify the color set in `color-rgb`. This value must be in the `0..1` range
* **red**
  Sets the red channel of the set color. This will override the `r` value in `color-rgb`, or will modify the color set in `color-hsv`. This value must be in the `0..255` range
* **green**
  Sets the green channel of the set color. This will override the `g` value in `color-rgb`, or will modify the color set in `color-hsv`. This value must be in the `0..255` range
* **blue**
  Sets the blue channel of the set color. This will override the `b` value in `color-rgb`, or will modify the color set in `color-hsv`. This value must be in the `0..255` range
* **border-color**
  Defines the color of the border in RGB. An alias for `border-color-rgb`. Please see the [colors](../Graphics/Colors.md) documentation for valid color values
* **border-color-rgb**
  Defines the color of the border in RGB. If `border-color-hsv` is also set, `border-color-rgb` will override this value. Please see the [colors](../Graphics/Colors.md) documentation for valid color values
* **border-color-hsv**
  Defines the color of the border in HSV. If `border-color-rgb` is also set, that will override this value. Please see the [colors](../Graphics/Colors.md) documentation for valid color values
* **border-hue**
  Defines the hue of the set border color. This will override the `h` value in `border-color-hsv` or will modify the color set in `border-color-rgb`. This value must be in the `0..1` range
* **border-saturation**
  Defines the saturation of the set border color. This will override the `s` value in `border-color-hsv` or will modify the color set in `border-color-rgb`. This value must be in the `0..1` range
* **border-brightness**
  Defines the brightness of the set border color. This will override the `v` value in `border-color-hsv` or will modify the color set in `border-color-rgb`. This value must be in the `0..1` range
* **border-red**
  Sets the red channel of the set border color. This will override the `r` value in `border-color-rgb`, or will modify the color set in `border-color-hsv`. This value must be in the `0..255` range
* **border-green**
  Sets the green channel of the set border color. This will override the `g` value in `border-color-rgb`, or will modify the color set in `border-color-hsv`. This value must be in the `0..255` range
* **border-blue**
  Sets the blue channel of the set border color. This will override the `b` value in `border-color-rgb`, or will modify the color set in `border-color-hsv`. This value must be in the `0..255` range
* **texture** The texture to be used. This is an alias for `texture[0]`
* **texture[n]** The texture to be used at the index `n`. Some materials allow for multiple textures. If a material does not support the texture index, this value will be ignored
* **margin** The amount of space surrounding this UI control. Adjacent UI controls will be rendered outside this space. Please note that adjacent UI controls' margins will also be taken into account when they are rendered  
* **padding** The amount of space within this UI control. Child controls will not be rendered inside of this space. Please note that child controls' margins will be taken into account when they are rendered

Built in controls and custom controls may define other attributes.

Unknown attributes will be ignored.

### Default CSS
You can find the default CSS styles in `./data/client/ui/css/default/`

### Creating Styles
A CSS style will be applied based on its class or type.

You can define a class as follows:

```css
.my-class {
    color-rgb: "red";
}
```

You can define a type as follows:

```css
button {
    texture: "{ClientUI}/ui_textures/button.png";
}
```

Type names are what you would define in the `type` field, or `name` field for custom controls, in [UI files](UIFiles.md). Please note that custom controls may apply CSS styles however they wish, and may override any global styles applied to their internal controls.

Attributes specified in custom CSS will override the global attributes.

### Styling UI Controls
You can apply styles to UI controls by using either the `style` or `cssClass` fields in a UI file.

The `style` field allows you to define local styles, for instance:

```json
{
  "id": "controlId",
  "type": "texture",
  "style": "color: 'red'; texture: 'path/to/texture.png"
}
```

The `cssClass` field allows you to assign a number of CSS classes to a UI control. CSS classes specified to the right will override any attributes specified in CSS classes to the left. For instance:

```css
.my-class-1 {
  color-rgb: "blue";
}

.my-class-2 {
  color-rgb: "red";
}
```

```json
{
  "id": "controlId",
  "type": "texture",
  "cssClass": "my-class-1 my-class-2"
}
```

The resulting `color` attribute will be that defined by `my-class-2`.

If you specify both the `style` and `cssClass` fields, the `style` field will override any attributes specified by classes in the `cssClass` field.
