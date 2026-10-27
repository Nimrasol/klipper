> **Snapmaker J1 fork.**  This branch (`j1`) is Klipper with the support
> for the main controller of the Snapmaker J1 as commits on top of
> Klipper: the GD32F305 chip and the J1 factory bootloader.  It belongs to
> the project
> [Klipper on the Snapmaker J1](https://github.com/Nimrasol/KlipperSnapmakerJ1).
>
> | Repository | Role in the project |
> | --- | --- |
> | [KlipperSnapmakerJ1](https://github.com/Nimrasol/KlipperSnapmakerJ1) | **Start here.** Everything printer specific: Klipper modules and configuration, the KlipperScreen calibration menu and theme, the main controller firmware, the user manual, downloads and installation |
> | [KlipperSnapmakerJ1-os](https://github.com/Nimrasol/KlipperSnapmakerJ1-os) | The Linux system of the display module, which runs Klipper, Moonraker, KlipperScreen and Mainsail on the printer |
> | **Nimrasol/klipper** (this one), branch `j1` | The main controller support as Klipper commits, for reference and review |
>
> The printer runs unmodified Klipper from
> [Klipper3d/klipper](https://github.com/Klipper3d/klipper).  The same
> changes are in KlipperSnapmakerJ1 as a patch
> (`firmware/klipper-gd32f305-snapmaker.patch`), which builds the main
> controller firmware from it.  Everything else on this page is the
> README of Klipper.

Welcome to the Klipper project!

[![Klipper](docs/img/klipper-logo-small.png)](https://www.klipper3d.org/)

https://www.klipper3d.org/

The Klipper firmware controls 3d-Printers. It combines the power of a
general purpose computer with one or more micro-controllers. See the
[features document](https://www.klipper3d.org/Features.html) for more
information on why you should use the Klipper software.

Start by [installing Klipper software](https://www.klipper3d.org/Installation.html).

Klipper software is Free Software. See the [license](COPYING) or read
the [documentation](https://www.klipper3d.org/Overview.html). We
depend on the generous support from our
[sponsors](https://www.klipper3d.org/Sponsors.html).
