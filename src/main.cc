#include "sian.hh"
#include "circle.hh"
#include "line.hh"
#include "horizontal_layout.hh"
#include "rectangle.hh"
#include "rectangle_container.hh"

#include <cmath>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <tuple>

using namespace Sian;

int main(int argc, char* argv[])
{
    Config conf = Config::from_args(argc, argv);
    Scene sc(conf);
    Animator anim(conf, sc);

    // ** Horizontal Layout **
    {
        auto c_1 = std::make_shared<Circle>(50);
        auto c_2 = std::make_shared<Circle>(50);
        auto c_3 = std::make_shared<Circle>(50);
        auto r = std::make_shared<Rectangle>(250, 100);

        auto layout = std::shared_ptr<HorizontalLayout>(
                new HorizontalLayout(Offset(300, 150), {c_1, c_2, c_3, r}));

        auto container = std::make_shared<RectangleContainer>(layout, 0);
        container->offset = Offset(25, 75);
        sc.add(container);

        for (auto c : std::initializer_list<std::shared_ptr<Shape>> {c_1, c_2, c_3, r})
        {
            c->line_width.animate_to(20.0, Duration(4));
            c->color = Color::rgb(0, 255, 0);
        }
        r->rotation.animate_to(2.0, Duration(3.0));
        r->scale_x.animate_to(0.5, Duration(2.0));
        r->scale_y.animate_to(0.5, Duration(2.0));
        r->color.animate_to(Color::rgb(0xAABBCC), Duration(4.0));
    }

    // ** Line **
    {
        auto line = std::make_shared<Line>(Offset(25, 250), Offset(550, 250));
        sc.add(line);
    }

    // ** Animating position **
    {
        auto circle = std::make_shared<Circle>(30);
        auto container = std::make_shared<RectangleContainer>(circle, 0);
        container->offset
            .set(Offset(300, 300))
            .animate_to(Offset(0, 300), Speed(250))
            .then_animate_to(Offset(0, 500), Speed(250))
            .then([circle] {
                    circle->color.set(Color::rgb(255, 0, 255));
                });
        sc.add(container);
    }

    // ** Using Speed **
    {
        auto car_1 = std::make_shared<Rectangle>(100, 75);
        auto car_2 = std::make_shared<Rectangle>(100, 75);
        car_1->offset
            .set(Offset(0, 400))
            .animate_to(Offset(500, 400), Speed(100));
        car_2->offset
            .set(Offset(0, 500))
            .animate_to(Offset(250, 500), Speed(100));
        sc.add({car_1, car_2});
    }

    // ** Connecting values **
    {
        auto wheel = std::make_shared<Circle>(20);
        auto line = std::make_shared<Line>(Offset(0, 0), Offset(400, 400));
        line->start.connect(
                wheel->offset,
                [] (Offset p) { return p.plus_x(20).plus_y(20); },
                [] (Offset p) { return p.minus_x(20).minus_y(20); });
        wheel->offset
            .bind([] (double t) {
                    const int x = 400 + 100 * std::cos(-t);
                    const int y = 400 + 100 * std::sin(-t);
                    return Offset(x, y);
                }, Duration(1.0))
            .then([line] () {
                    line->start.animate_to(Offset(25, 280), Duration(4.0));
                });
        sc.add({wheel, line});
    }

    // ** Using different parameters for setting position **
    {
        auto circle = std::make_shared<Circle>(Offset(800, 100), 50);
        circle->offset
            .animate_to(Offset(800, 100), Duration(2.0))
            .then([circle] () {
                    circle->center.animate_to(Offset(800, 100), Duration(2.0));
                });
        sc.add(circle);
    }

    // ** Car! **
    {
        auto left_wheel = std::make_shared<Rectangle>(35, 35);
        auto right_wheel = std::make_shared<Rectangle>(35, 35);
        auto car = std::make_shared<Rectangle>(150, 75);

        left_wheel->center.connect(
                car->bottom_left,
                [] (Offset car_corner) {
                    return car_corner.plus_x(35);
                },
                [] (Offset wheel_center) {
                    return wheel_center.minus_x(35);
                });
        right_wheel->center.connect(
                car->bottom_right,
                [] (Offset car_corner) {
                    return car_corner.minus_x(35);
                },
                [] (Offset wheel_center) {
                    return wheel_center.plus_x(35);
                });

        car->offset
            .set(Offset(200, 550), Duration(0.5)) // let the engine warm up
            .animate_to(Offset(1000, 550), Speed(175));
        left_wheel->rotation
            .set(0, Duration(0.5))
            .then_bind([] (double t) { return t * 4; });
        right_wheel->rotation
            .set(0, Duration(0.5))
            .then_bind([] (double t) { return t * 4; });

        sc.add({car, left_wheel, right_wheel});
    }

    anim.wait(5);
    anim.finish();
}
