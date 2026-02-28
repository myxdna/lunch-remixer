// Lunch Remixer Enclosure — LILYGO T5 4.7" e-Paper (ESP32-S3)
// LiPo: 503040 (40×30×5mm), JST PH 2.0mm
// Fridge mounting: 3× neodymium disc magnets (10mm × 2mm)
// Closed with 4× M3 socket cap head screws + M3 heat-set threaded inserts
// Designed for Bambu Lab P1S, PLA — NO SUPPORTS REQUIRED
//
// Can be used as a library: include <lunch_remixer_case.scad>
// then set LIBRARY_MODE = true (after the include) to suppress the exploded render.
LIBRARY_MODE = false;

/* --- Board measurements (mm) --- */

// LILYGO T5 4.7" v2.3
board_length = 121.0;
board_width  = 67.0;
board_thick  = 5.0;    // PCB + back components; display is flush on front face

// Display active area
display_length   = 105.0;
display_width    = 60.0;
display_offset_x = 8.0;   // from board left edge
display_offset_y = 3.5;   // from board bottom edge

// USB-C port (on the LEFT short edge of the board — x=0 wall)
usbc_width    = 9.5;
usbc_height   = 3.5;
usbc_offset_y = 33.5;   // center of port along the short edge

// Button (GPIO 21, near right edge of board top face)
button_x        = 110.0;
button_y        = 10.0;
button_diameter = 4.0;

// LiPo battery — 503040 (40×30×5mm, JST PH 2.0mm)
lipo_length = 40.0;
lipo_width  = 30.0;
lipo_thick  = 5.0;

// Neodymium magnets — 10mm × 2mm disc
magnet_diameter = 10.0;
magnet_height   = 2.0;    // exactly 2mm thick

/* --- Case geometry --- */

wall      = 2.5;    // 2.5mm walls — gives 0.5mm floor under blind magnet pocket
clearance = 0.3;
corner_r  = 3.0;

// Stack from fridge side (back) to display side (front):
//   back wall (2.5) + lipo zone (6.0) + board zone (5.5) + front wall (2.5)
back_zone  = lipo_thick + 1.0;    // 6.0mm  — lipo + air gap
board_zone = board_thick + 0.5;   // 5.5mm  — board + clearance

total_depth = wall + back_zone + board_zone + wall;  // 16.5mm

inner_length = board_length + clearance * 2;
inner_width  = board_width  + clearance * 2;
outer_length = inner_length + wall * 2;
outer_width  = inner_width  + wall * 2;

// Split plane: back shell holds LiPo + bottom of board zone
// Front shell holds the board and display
split_at = wall + back_zone + 2.0;   // 10.5mm back / 6.0mm front

/* --- Screw boss geometry --- */
// 4× M3 socket cap head screws + M3 heat-set threaded inserts
// Inserts: heated into front shell boss blind pockets from mating face
// Screws: M3 cap head from fridge face, clearance through back shell posts

boss_od       = 8.0;   // outer diameter — wall around M3 insert pocket
boss_inset    = 6.5;   // boss centre from outer corner edge
screw_clr_d   = 3.3;   // M3 clearance hole through back shell
csink_d       = 6.0;   // M3 socket cap head recess diameter
csink_z       = 3.2;   // M3 socket cap head height recess
insert_hole_d = 4.2;   // M3 heat-set insert press hole in PLA
insert_depth  = 5.5;   // blind pocket depth (≥ 5mm insert length)

boss_positions = [
    [boss_inset,                boss_inset],
    [outer_length - boss_inset, boss_inset],
    [boss_inset,                outer_width - boss_inset],
    [outer_length - boss_inset, outer_width - boss_inset]
];

// Magnet pocket positions (3 evenly spaced, centered on back wall)
magnet_positions = [
    [outer_length * 0.20, outer_width / 2],
    [outer_length * 0.50, outer_width / 2],
    [outer_length * 0.80, outer_width / 2]
];

/* --- Teenage Engineering-style FRONT face design --- */
// All branding is on the user-visible display side.
// Back face (fridge) is clean: only magnet pockets + screw recesses.

// Dot matrix — left strip of front shell, beside the display window
// Matches K.O. II grille aesthetic; 2 columns of blind pockets
dot_d      = 1.8;    // blind pocket diameter (printable at 0.20mm layers)
dot_depth  = 0.8;    // blind pocket depth (leaves ≥1.7mm floor in 2.5mm wall)
dot_pitch  = 4.5;    // centre-to-centre spacing
dot_area_x = 3.0;    // left column x in left border strip
dot_area_w = 5.0;    // span ~2 columns (x = 3.0 and 7.5)
dot_area_y = 14.0;   // y start — clears corner bosses (boss at y=6.5, r=4mm)
dot_area_h = outer_width - 28.0;  // clears both corner bosses

// Engraved product name — bottom border of front face (display side)
// Below the display window, centred; readable when device is on fridge door
name_x = outer_length / 2;   // horizontally centred
name_y = 2.9;                 // centre of bottom border strip (~5.8mm tall)

/* --- USB-C cutout dimensions ---
   Board sits at z = wall + back_zone = 8.5mm from fridge face.
   USB-C port center ≈ 1.75mm above PCB bottom = z ≈ 10.25mm.
   Port spans z ≈ 8.5mm to 12.0mm — crosses the split plane at 10.5mm.
   Cut the slot in both shells so it aligns. */
usbc_slot_z_back  = wall + back_zone - 0.5;   // start in back shell (z=8.0)
usbc_slot_height  = usbc_height + 1.5;         // 5.0mm — generous

/* --- Helper module --- */
module rounded_box(l, w, h, r) {
    hull() {
        for (x = [r, l - r])
            for (y = [r, w - r])
                translate([x, y, 0])
                    cylinder(h = h, r = r, $fn = 24);
    }
}

/* =============================================================
   BACK SHELL
   Print orientation: open-side UP, fridge face on build plate
   No overhangs > 45°, no supports needed
   ============================================================= */
module back_shell() {
    difference() {
        union() {
            rounded_box(outer_length, outer_width, split_at, corner_r);

            // Boss columns for M3 screws — solid cylinders, trimmed by interior later
            for (p = boss_positions)
                translate([p[0], p[1], 0])
                    cylinder(d = boss_od, h = split_at, $fn = 24);
        }

        // ---- Interior cavity ----
        translate([wall, wall, wall])
            cube([inner_length, inner_width, split_at - wall + 0.1]);

        // ---- Blind magnet pockets (from fridge face) ----
        // wall = 2.5mm, magnet = 2.0mm → 0.5mm floor remains. Press-fit from inside after printing.
        for (m = magnet_positions)
            translate([m[0], m[1], -0.1])
                cylinder(d = magnet_diameter + 0.4, h = magnet_height + 0.1, $fn = 32);

        // ---- M3 screw clearance holes + cap-head recesses (fridge face) ----
        for (p = boss_positions) {
            // M3 clearance hole through full post depth
            translate([p[0], p[1], -0.1])
                cylinder(d = screw_clr_d, h = split_at + 0.2, $fn = 16);
            // Cylindrical recess for M3 socket cap head
            translate([p[0], p[1], -0.1])
                cylinder(d = csink_d, h = csink_z + 0.1, $fn = 16);
        }

        // ---- USB-C slot (left short wall, x=0 face) ----
        translate([-0.1,
                   wall + clearance + usbc_offset_y - usbc_width / 2,
                   usbc_slot_z_back])
            cube([wall + 0.2, usbc_width, usbc_slot_height]);
    }

    // ---- LiPo retainer posts ----
    // 4 corner posts hold the 503040 battery flat in its pocket
    lx = wall + (inner_length - lipo_length) / 2;
    ly = wall + (inner_width  - lipo_width)  / 2;
    post = 2.5;
    for (dx = [0, lipo_length - post])
        for (dy = [0, lipo_width - post])
            translate([lx + dx, ly + dy, wall])
                cube([post, post, lipo_thick + 0.5]);

    // ---- Board support ledges ----
    // Two rails the board rests on, above the LiPo zone
    ledge_z = wall + back_zone;
    for (y_off = [wall + 2, outer_width - wall - 4]) {
        translate([wall + 18, y_off, ledge_z])
            cube([inner_length - 36, 2.0, 1.5]);
    }
}

/* =============================================================
   FRONT SHELL
   Print orientation: flat face (display side) DOWN on build plate
   Open/mating face prints upward — no supports needed
   ============================================================= */
module front_shell() {
    fh = total_depth - split_at;   // 6.0mm front shell height

    difference() {
        union() {
            rounded_box(outer_length, outer_width, fh, corner_r);

            // Boss columns for M2 pilot holes
            for (p = boss_positions)
                translate([p[0], p[1], 0])
                    cylinder(d = boss_od, h = fh, $fn = 24);
        }

        // ---- Interior pocket — board slides in from mating face ----
        translate([wall, wall, 0])
            cube([inner_length, inner_width, fh - wall]);

        // ---- Display window ----
        wx = wall + clearance + display_offset_x - 0.5;
        wy = wall + clearance + display_offset_y - 0.5;
        translate([wx, wy, fh - wall - 0.1])
            cube([display_length + 1, display_width + 1, wall + 0.2]);

        // ---- Button access hole (GPIO 21) ----
        translate([wall + clearance + button_x,
                   wall + clearance + button_y, -0.1])
            cylinder(d = button_diameter + 1.5, h = fh + 0.2, $fn = 20);

        // ---- USB-C slot continuation (aligns with back shell cutout) ----
        // Back shell cuts from z=8.0mm; front shell bottom is at split_at=10.5mm.
        // Front shell USB-C slot covers the remaining port height.
        translate([-0.1,
                   wall + clearance + usbc_offset_y - usbc_width / 2,
                   0])
            cube([wall + 0.2, usbc_width, usbc_slot_height - (split_at - usbc_slot_z_back) + 0.1]);

        // ---- M3 heat-set insert blind pockets (from mating face into boss) ----
        // Heat insert from mating/interior side; screw enters from fridge face.
        for (p = boss_positions)
            translate([p[0], p[1], -0.1])
                cylinder(d = insert_hole_d, h = insert_depth + 0.1, $fn = 16);

        // ---- Dot matrix (front/display face, left border strip) ----
        // 2 columns of blind pockets beside the display window — K.O. II aesthetic.
        for (ix = [0 : floor(dot_area_w / dot_pitch)])
            for (iy = [0 : floor(dot_area_h / dot_pitch)])
                translate([dot_area_x + ix * dot_pitch,
                           dot_area_y  + iy * dot_pitch,
                           fh - dot_depth])
                    cylinder(d = dot_d, h = dot_depth + 0.1, $fn = 12);

        // ---- Engraved product name (front/display face, bottom border) ----
        // Centred below the display window; readable when device is on the fridge.
        translate([name_x, name_y, fh - 0.6])
            linear_extrude(0.7)
                text("LUNCH REMIXER",
                     size = 3.5,
                     font = "Liberation Mono:style=Bold",
                     halign = "center",
                     valign = "center");
    }
}

/* --- Render (exploded view — suppressed when LIBRARY_MODE = true) --- */

if (!LIBRARY_MODE) {
    color("SteelBlue")
        back_shell();

    color("LightSteelBlue")
        translate([0, outer_width + 20, 0])
            front_shell();

    echo(str("OUTER SIZE: ", outer_length, " × ", outer_width, " × ", total_depth, " mm"));
    echo(str("BACK SHELL: ", split_at, " mm   FRONT SHELL: ", total_depth - split_at, " mm"));
    echo(str("SCREWS: 4× M3 socket cap head + M3 heat-set threaded inserts"));
}

// --- To export STL, uncomment ONE line (and set LIBRARY_MODE = false above) ---
// back_shell();
// front_shell();
