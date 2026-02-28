// Lunch Remixer Enclosure — LILYGO T5 4.7" e-Paper (ESP32-S3)
// Ultra-slim: single board + thin LiPo behind it
// Neodymium magnets for fridge mounting
// Designed for Bambu Lab P1S, PLA, no supports needed

/* --- Board measurements (mm) --- */

// LILYGO T5 4.7" v2.3 overall
board_length = 121.0;
board_width  = 67.0;
board_thick  = 5.0;    // PCB + components on back (display is flush on front)

// Display visible area (4.7" diagonal, 960x540)
// Approximate active area based on standard 4.7" e-paper
display_length = 105.0;
display_width  = 60.0;
display_offset_x = 8.0;   // from board left edge
display_offset_y = 3.5;   // from board bottom edge

// USB-C port (on the short edge)
usbc_width  = 9.5;
usbc_height = 3.5;
usbc_offset_y = 33.5;  // centered-ish on the short edge

// Button (GPIO 21, on the board edge)
button_x = 110.0;  // approximate, near the right edge
button_y = 10.0;
button_diameter = 4.0;

// Mounting holes (3.8mm, 6 total — approximate positions)
mount_hole_d = 3.8;

// LiPo battery (103040 — 40x30x5mm, ~1000mAh, common thin cell)
// Sits flat behind the PCB
lipo_length = 40.0;
lipo_width  = 30.0;
lipo_thick  = 5.0;

// Neodymium magnets (10mm x 2mm thin discs)
magnet_diameter = 10.0;
magnet_height = 2.0;
magnet_count = 3;

/* --- Case parameters --- */

wall = 1.5;
clearance = 0.3;
corner_r = 3.0;

// Layers from back (fridge side) to front (display side):
//   back wall (1.5) + lipo (5.0) + gap (0.5) + board back components (5.0)
//                    + display flush to front wall (1.5)
// But since the display is on the FRONT of the board, the board sits with
// display facing the front shell. Components and LiPo go behind.

back_zone = lipo_thick + 1.0;  // 6mm: lipo + clearance
board_zone = board_thick + 0.5; // 5.5mm: board + clearance

total_depth = wall + back_zone + board_zone + wall;  // ~14.5mm!

inner_length = board_length + clearance * 2;
inner_width  = board_width + clearance * 2;

outer_length = inner_length + wall * 2;
outer_width  = inner_width + wall * 2;
outer_depth  = total_depth;

// Two-piece split
split_at = wall + back_zone + board_zone * 0.3;

/* --- Modules --- */

module rounded_box(l, w, h, r) {
    hull() {
        for (x = [r, l - r])
            for (y = [r, w - r])
                translate([x, y, 0])
                    cylinder(h = h, r = r, $fn = 24);
    }
}

// Back shell — LiPo + magnets + board support
module back_shell() {
    difference() {
        rounded_box(outer_length, outer_width, split_at, corner_r);

        // Interior cavity
        translate([wall, wall, wall])
            cube([inner_length, inner_width, split_at]);

        // Magnet pockets (recessed into back wall, accessed from inside)
        magnet_spacing = outer_length / (magnet_count + 1);
        for (i = [1 : magnet_count]) {
            translate([magnet_spacing * i, outer_width / 2, -0.1])
                cylinder(d = magnet_diameter + 0.3, h = magnet_height + 0.3, $fn = 30);
        }

        // USB-C cutout (on short edge where the port is)
        translate([-0.1, wall + clearance + usbc_offset_y - usbc_width / 2,
                   wall + back_zone])
            cube([wall + 0.2, usbc_width, usbc_height + 1]);
    }

    // LiPo retainer corner posts
    lipo_x = wall + (inner_length - lipo_length) / 2;
    lipo_y = wall + (inner_width - lipo_width) / 2;
    post = 2;
    for (dx = [0, lipo_length - post])
        for (dy = [0, lipo_width - post])
            translate([lipo_x + dx, lipo_y + dy, wall])
                cube([post, post, lipo_thick]);

    // Board support ledges (board rests on these, above the lipo zone)
    ledge_z = wall + back_zone;
    for (y_off = [wall + 3, outer_width - wall - 4]) {
        translate([wall + 10, y_off, ledge_z])
            cube([inner_length - 20, 1.2, 1.0]);
    }

    // Snap ridges
    ridge_z = split_at - 1.0;
    for (y = [-0.5, outer_width]) {
        translate([outer_length * 0.15, y, ridge_z])
            cube([12, 0.5, 1.0]);
        translate([outer_length * 0.50, y, ridge_z])
            cube([12, 0.5, 1.0]);
        translate([outer_length * 0.85, y, ridge_z])
            cube([12, 0.5, 1.0]);
    }
}

// Front shell — display window + button access
module front_shell() {
    front_height = outer_depth - split_at;

    difference() {
        rounded_box(outer_length, outer_width, front_height, corner_r);

        // Interior cavity
        translate([wall, wall, -0.1])
            cube([inner_length, inner_width, front_height - wall + 0.1]);

        // Display window (generous opening for the e-paper)
        window_x = wall + clearance + display_offset_x - 0.5;
        window_y = wall + clearance + display_offset_y - 0.5;
        translate([window_x, window_y, front_height - wall - 0.1])
            cube([display_length + 1, display_width + 1, wall + 0.2]);

        // Button hole (GPIO 21)
        translate([wall + clearance + button_x, wall + clearance + button_y, -0.1])
            cylinder(d = button_diameter + 1.5, h = front_height + 0.2, $fn = 20);

        // Snap grooves
        tol = 0.15;
        for (y = [-tol, outer_width - 0.5 + tol]) {
            translate([outer_length * 0.15 - tol, y, front_height - 1.0 - tol])
                cube([12 + tol * 2, 0.5 + tol, 1.0 + tol + 0.1]);
            translate([outer_length * 0.50 - tol, y, front_height - 1.0 - tol])
                cube([12 + tol * 2, 0.5 + tol, 1.0 + tol + 0.1]);
            translate([outer_length * 0.85 - tol, y, front_height - 1.0 - tol])
                cube([12 + tol * 2, 0.5 + tol, 1.0 + tol + 0.1]);
        }
    }

    // Display bezel
    window_x = wall + clearance + display_offset_x - 0.5;
    window_y = wall + clearance + display_offset_y - 0.5;
    bezel = 1.0;
    difference() {
        translate([window_x - bezel, window_y - bezel, front_height - wall - 0.3])
            cube([display_length + 1 + bezel * 2, display_width + 1 + bezel * 2, 0.3]);
        translate([window_x, window_y, front_height - wall - 0.4])
            cube([display_length + 1, display_width + 1, 0.5]);
    }
}

/* --- Render (exploded view) --- */

color("SteelBlue")
    back_shell();

color("LightSteelBlue")
    translate([0, outer_width + 15, 0])
        front_shell();

echo(str("TOTAL THICKNESS: ", outer_depth, " mm"));
echo(str("OUTER SIZE: ", outer_length, " x ", outer_width, " x ", outer_depth, " mm"));

// --- To export STL: uncomment ONE ---
// back_shell();
// front_shell();
