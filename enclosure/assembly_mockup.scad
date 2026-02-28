// Lunch Remixer — Fully Assembled Device Mockup
// -----------------------------------------------
// Open in OpenSCAD → F5 (preview) or F6 (render).
//
// Shows both shells assembled in final position with internal
// components visible through semi-transparent PLA shells.
//
// Z axis orientation:
//   z = 0           → fridge face (magnets, screws, dot matrix, engraving)
//   z = total_depth → display face (user-visible side)
//
// Import the case file as a library; LIBRARY_MODE = true (defined below,
// after the include) suppresses the exploded-view render in that file.

include <lunch_remixer_case.scad>
LIBRARY_MODE = true;   // overrides LIBRARY_MODE = false from included file

/* ---- Material colours ----------------------------------------- */

// Case shells — warm off-white PLA, semi-transparent to show internals
SHELL_C    = [0.91, 0.89, 0.85, 0.72];

// Internal components
LIPO_C     = [0.15, 0.30, 0.55, 1.00];   // LiPo wrap (dark blue)
CONNECTOR_C= [0.92, 0.92, 0.92, 1.00];   // JST connector (white)
PCB_C      = [0.18, 0.42, 0.18, 1.00];   // PCB substrate (green)
EPAPER_C   = [0.95, 0.95, 0.93, 1.00];   // e-ink panel (off-white)
CHIP_C     = [0.22, 0.22, 0.22, 1.00];   // ESP32 module (dark grey)
METAL_C    = [0.65, 0.65, 0.65, 1.00];   // screws, magnets, USB-C (silver)
BUTTON_C   = [0.18, 0.18, 0.18, 1.00];   // tactile button cap (black)

/* ---- Derived positioning --------------------------------------- */

// Board sits on the support ledges in the back shell.
// ledge_z = wall + back_zone = 2.5 + 6.0 = 8.5mm (base of ledge)
// ledge height = 1.5mm  → PCB component face rests at 10.0mm
_ledge_z  = wall + back_zone;
_board_z  = _ledge_z + 1.5;           // 10.0mm — PCB component-side bottom
_pcb_top  = _board_z + 1.6;           // 11.6mm — PCB substrate top
_disp_z   = _board_z + board_thick - 0.8; // display face z (top of board stack)

/* ================================================================
   COMPONENT MODULES
   ================================================================ */

// LiPo 503040 battery in battery bay
module lipo_component() {
    lx = wall + (inner_length - lipo_length) / 2;
    ly = wall + (inner_width  - lipo_width)  / 2;

    // Main cell body
    color(LIPO_C)
        translate([lx, ly, wall + 0.3])
            cube([lipo_length, lipo_width, lipo_thick - 0.3]);

    // JST PH 2.0mm connector stub (exits toward right short edge)
    color(CONNECTOR_C)
        translate([lx + lipo_length,
                   ly + lipo_width / 2 - 4,
                   wall + lipo_thick - 3])
            cube([7, 8, 5]);

    // Polarity stripe on cell top face
    color([0.55, 0.20, 0.20, 1.0])
        translate([lx, ly, wall + lipo_thick])
            cube([6, lipo_width, 0.2]);
}

// LILYGO T5 4.7" v2.3 board — PCB substrate + display + key components
module pcb_component() {
    px = wall + clearance;
    py = wall + clearance;

    // PCB substrate (green, 1.6mm thick)
    color(PCB_C)
        translate([px, py, _board_z])
            cube([board_length, board_width, 1.6]);

    // e-ink display panel on PCB front face
    color(EPAPER_C)
        translate([px + display_offset_x,
                   py + display_offset_y,
                   _pcb_top])
            cube([display_length, display_width, 0.6]);

    // Display bezel frame (thin border around panel)
    color([0.55, 0.55, 0.55, 1.0]) {
        translate([px + display_offset_x - 1.5,
                   py + display_offset_y - 1.5,
                   _pcb_top])
            difference() {
                cube([display_length + 3, display_width + 3, 0.8]);
                translate([1.5, 1.5, -0.1])
                    cube([display_length, display_width, 1.0]);
            }
    }

    // ESP32-S3 module on PCB back face (approximate footprint 18×14mm)
    color(CHIP_C)
        translate([px + board_length / 2 - 9,
                   py + board_width  / 2 - 7,
                   _board_z - 3.4])
            cube([18, 14, 3.4]);

    // USB-C port on left short edge
    color(METAL_C)
        translate([px - 4,
                   py + usbc_offset_y - usbc_width / 2,
                   _board_z + 0.6])
            cube([5, usbc_width, usbc_height]);

    // Tactile button (GPIO 21) — near right edge of board top
    color(BUTTON_C)
        translate([px + button_x - button_diameter / 2,
                   py + button_y - button_diameter / 2,
                   _pcb_top])
            cylinder(d = button_diameter, h = 2.0, $fn = 16);
    color(METAL_C)
        translate([px + button_x,
                   py + button_y,
                   _pcb_top + 2.0])
            cylinder(d = 2.0, h = 0.8, $fn = 16);

    // Ribbon cable from display to PCB edge (suggestive only)
    color([0.80, 0.80, 0.10, 0.8])
        translate([px + display_offset_x + display_length * 0.4,
                   py,
                   _board_z + 0.8])
            cube([display_length * 0.2, 2, 0.4]);
}

// Neodymium disc magnets pressed into back wall pockets
module magnet_components() {
    for (m = magnet_positions)
        color(METAL_C)
            translate([m[0], m[1], 0.6])
                cylinder(d = magnet_diameter - 0.6,
                         h = magnet_height - 0.6,
                         $fn = 28);
}

// M3 socket cap head screws recessed into fridge face
module screw_components() {
    for (p = boss_positions)
        color(METAL_C) {
            // Cap head cylinder sitting in cylindrical recess
            translate([p[0], p[1], 0.1])
                cylinder(d = csink_d - 0.6, h = csink_z - 0.2, $fn = 16);
            // Hex socket recess (suggestive — 6-sided)
            color([0.28, 0.28, 0.28, 1.0])
                translate([p[0], p[1], csink_z - 0.2])
                    cylinder(d = 2.5, h = 0.5, $fn = 6);
        }
}

/* ================================================================
   ASSEMBLY
   ================================================================ */

// Back shell (fridge-facing, z=0 at fridge face)
color(SHELL_C) back_shell();

// Internal components — visible through semi-transparent back shell
lipo_component();
pcb_component();
magnet_components();
screw_components();

// Front shell — assembled on top of back shell
color(SHELL_C)
    translate([0, 0, split_at])
        front_shell();

/* ================================================================
   DIMENSIONS
   ================================================================ */
echo(str("=== LUNCH REMIXER — ASSEMBLED MOCKUP ==="));
echo(str("Outer:        ", outer_length, " × ", outer_width, " × ", total_depth, " mm"));
echo(str("Back shell:   ", split_at, " mm deep"));
echo(str("Front shell:  ", total_depth - split_at, " mm deep"));
echo(str("Board bottom: z = ", _board_z, " mm"));
echo(str("Board top:    z = ", _board_z + board_thick, " mm"));
echo(str("z = 0         → fridge face"));
echo(str("z = ", total_depth, "      → display face"));
