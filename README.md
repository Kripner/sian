# Sian<br />Simple animation library

## Overview

Sian is designed for intuitive creation of animations consisting of geometric objects (text and images will be added in future
updates). The library delegates all the rendering to [Cairo](https://www.cairographics.org/) and therefore inherits many of its approaches and limitations.
However, Sian strives to be at least as powerful as Cairo. The features that aren't accessible through Sian's interface
are exposed directly, as the user can create custom objects and define how they should be rendered using Cairo.

The only way to export your animation in the current version is using [FFmpeg](https://www.ffmpeg.org/). Sian exports the frames one by one as PNG images
and delegates creation of the resultant MP4 video to FFmpeg, which is assumed to be installed on the system. Others formats and
more intelligent means of exporting the animations will be added in the future.

While Sian can be used as a part of a more complex system, it is also designed to be directly usable as a standalone animation tool.
Anyone with basic knowledge of C++ syntax can write a *script* for the animation and *compile* it using Sian. The resulting executable
provides a simple text-based user interface with the usual structure of a command-line tool.

## Installation and Usage

1) Prerequisites

Install [Cairo](https://www.cairographics.org/download/) and [FFmpeg](https://www.ffmpeg.org/download.html).

2) Compile and install Sian.

```
$ cd sian
$ cmake -H. -Bbuild
$ cd build
$ make all
```

3) Examine and run the created executable.

```
$ ./sample -help
$ mkdir pngs
$ ./sample
```

4) Use the library.


## The Basics

State of the animation at any moment is represented by an instance of `Sian::Scene`. Scene contains methods for advancing the state one
tick forward and snapshotting the current state. A snapshot can then be exported to PNG.
`Sian::Animator` is responsible for composing individual snapshots into animation and saving it in a standardized format.

Both `Sian::Scene` and `Sian::Animator` are configured via `Sian::Config`, representing a set of configurable parameters.
An instance of `Sian::Config` can generally be obtained in 2 ways:

1. From command-line arguments using `Config::from_args(argc, argv)`.
3. Creating Config manually (all parameters are automatically set to their default values) and overriding the values that
   you wish to customize.

Combining the information above, a typical Sian animation script will start with following lines in the main method:

```c++
Config conf = Config::from_args(argc, argv);
Scene sc(conf);
Animator anim(conf, sc);
```

It is important to understand that the animation is only started once a appropriate method on the Animator is called. Everything before that
simply enqueues appropriate instructions. In the current version, the said method to call is:

```c++
anim.wait(5);
```

Where the argument specifies how many seconds of animation should be generated.

To export the final animation, following method must be called:

```c++
anim.finish();
```


## Animated Values

Much of the heavy-lifting is done by `Sian::AnimatedValue`. Any property of an object in a scene, such as its position, color or
rotation, is internally an Animated Value. This means that all such properties can be manipulated with in a uniform manner.
The `Sian::AnimatedValue` is a class template parametrized by the type of its payload, i.e. type of the actual value. This will be
different for a position than for a color, for example.
Some of the payload types are:

- `double` - scalar value
- `Offset` - x and y coordinates, interpreted either as a position or as a vector
- `Color` - stored as RGB values

Any type can be used as payload type, as long as corresponding specialisations for the following two function templates are defined:

- `interpolate(origin, target, t)` - linearly interpolates between origin and target using parameter `t` in the range [0, 1]
- `distance(a, b)` - returns distance between two values of the payload type

The two shall be connected using the concept of *speed* or *rate of change*. If we query `interpolate()` in timestamps linearly increasing
with physical time, the rate of change as perceived by the user should be directly proportional to `distance()` between origin and target.
In other words, the implementations should correspond with an intuitive understanding of linear interpolation and distance.

A sample implementation for payload type Offset is given below.

```c++
template<>
Offset interpolate<Offset>(const Offset& origin, const Offset& target, double t)
{
    // uses interpolate<double>()
    return Offset(
            interpolate(origin.x, target.x, t),
            interpolate(origin.y, target.y, t));
}

template<>
double distance<Offset>(const Offset& a, const Offset& b)
{
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}
```

## Manipulating Animated values

The payload value inside an animated value can be controlled in 4 different ways: setting to a constant, animating it, binding
it to a function parameterized by time and connecting it to another animated value. All of them are discussed below.

### Constant value
AnimatedValue overloads `operator=(rhs)` and also provides `set(rhs)` method. Both of these have the effect of setting the internal value
to the provided constant.

### Animating
For changing the value smoothly over time, use the `animate_to(target, pace_value)` method. The first argument designates the desired final value
of the animation. The second argument specifies the pace of the animation, which can either be its duration or its speed. Use `Sian::Duration(desired_duration)`
or `Sian::Speed(desired_speed)` respectively. Duration is given in seconds. Speed is given in *natural units* per second, where a natural unit
is defined by the designer of the corresponding payload type. For Offset it's the width of 1 pixel, for double it's 1. For Color the speed should
be fine-tuned by the user and will generally be about 1.
Note that when we discuss *time*, what we mean is the ticking of a `Sian::Animator` rather than physical time. The animation itself will not take place
until the animator is instructed to generate the appropriate frames. The `animate_to()` method is therefore non-blocking, as it simply changes the state
of the animated value.

### Binding to function
It is possible to specify an explicit value provider that will be queried independently for each frame of the animation. Given the time elapsed since its creation,
its purpose is to calculate the payload value. Value provider can be registered using `bind(provider)` method and it should be a function from double to
payload type.

### Connecting to another animated value
Animated value can be connected to another animated value using `connect(anim_value)` method. After that, the payload values of the two animated values
will be stored in the same memory location, meaning that updating one of them will update the other one as well. This relationship is
symmetric and cannot be undone. Only animated values of the same payload type can be connected.

For example, imagine you want the end of a line to be always located in the center of a circle. You can connect these two properties. If you then
animate the circle to another position, the endpoint of the line will be changing as well.

You can also specify transition functions from one payload value to the other and vice versa (these two transition functions should be inverses of each other).
For example, you might want the end of the line to be always located 50 pixels below the center of the circle (see example below).

```c++
end_of_line.connect(
    center_of_circle,
    [] (Offset center) {
        return center.minus_y(50);
    },
    [] (Offset end) {
        return end.plus_y(50);
    });
```

After creating an arbitrary number of connections, the relationship between all animated values can be described by an undirected graph, where any two
animated values share an edge iff they have been explicitly connected. Any two animated values in a connected component will also behave as if
they have been connected explicitly.

### Example
All said methods return reference to the manipulated Animated Value, utilizing fluent interface. The first three also provide versions with *then_* prefix.
Their behaviour depends on the current state of the Animated Value. If there is no transition event planned (the value is set to a constant or bound to a function
without specifying a timeout), the prefix has no effect. Otherwise the instruction is pushed to a queue and its execution is postponed.

For the purpose of the following example, let's assume that `position` is an Animated Value with payload type `Offset`.

```c++
position
    .set(Offset(20, 20), Duration(2.0))
    .then_set(Offset(100, 0))
    .then_animate_to(Offset(500, 0)) // 'then_' is not necessary here
    .then_bind_to(
            [] (double t) {
                return Offset(100 + std::sin(t), 100 + std::cos(t));
            }); // no timeout given
```


## Objects

Anything that can be added to the scene is an Object. There are a number of these ready to be used:

- `Circle`
- `Line`
- `Rectangle`
- `RectangleContainer` - draws a rectangular border around another Object
- `HorizontalContainer` - adjusts positions of a list of Objects so that they appear in a horizontal row

More will be added in the future.
All Objects share a common interface comprising of several Animated Values representing properties of a given Object. These are:

- `color`
- `rotation` (in radians, positive values representing clockwise rotation)
- `completion` - animating this value from 0 to 1 creates the illusion of smoothly creating the Object (if the particular Object supports it)
- `scale_x` - scaling factor in the horizontal dimension
- `scale_y` - scaling factor in the vertical dimension

Following properties depend on the notion of *wrapper box* and *hull*. Hull is the smallest rectangle containing the whole Object. Wrapper box the smallest
rectangle containing the whole Object whose edges are parallel to the x- and y-axis. When rotation is not present, these terms coincide.

- `top_left`, `top_right`, `bottom_right`, `bottom_left` - corners of the the hull
- `offset` - upper left corner of the wrapper box
- `center` - center of the wrapper box (also used as center of rotation - therefore it's center of the hull as well)

All Objects also provide `x_dimension()` and `y_dimension()` methods calculating dimensions of wrapper box and `object_width()` and `object_height()` methods calculating
dimensions of hull.


## Creating Custom Objects

An Object is defined by a class inheriting from `Sian::Object`. Several methods have to or can be overridden:

```c++
void push_context(DrawContext cr) const [protected]
```
DrawContext is an alias for `cairo_t*`. The purpose of this method is to set up the Cairo drawing context. If overridden, it should begin with a call to
the ancestor's version of the method.

```c++
void draw(DrawContext cr) [public]
```
This method is responsible for rendering the Object in its current state. It should begin with a call to `push_context(cr)` and end with `pop_context(cr)`.
This ensures that the context is properly set up and then restored to its original state once the method exits.

```c++
double natural_width() const [protected]
double natural_height() const [protected]
```
These should return dimensions of the wrapper box when no scaling or rotation is present.

```c++
std::list<UpdatableValue*> animated_values() [public]
```
This method returns a list of pointers to all Animated Values associated with given Object - including those set up by ancestor classes. Sample implementation for
Rectangle is given below (apart from properties general to Shapes, Rectangle defines width and height).

```c++
std::list<UpdatableValue*> Rectangle::animated_values()
{
    auto values = Shape::animated_values();
    values.insert(values.end(), {&width, &height});
    return values;
}
```

## Known bugs

- Setting Line::start vs Line::end behave slightly asymmetrically.
