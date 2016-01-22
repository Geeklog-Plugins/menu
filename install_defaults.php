<?php

/* Reminder: always indent with 4 spaces (no tabs). */
// +---------------------------------------------------------------------------+
// | Menu Plugin 1.0                                                           |
// +---------------------------------------------------------------------------+
// | install_defaults.php                                                      |
// |                                                                           |
// | This file is used to hook into Geeklog's configuration UI                 |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2012 by the following authors:                              |
// |                                                                           |
// | Authors: Ben - ben AT geeklog DOT fr                                      |
// +---------------------------------------------------------------------------+
// | Created with the Geeklog Plugin Toolkit.                                  |
// +---------------------------------------------------------------------------+
// |                                                                           |
// | This program is free software; you can redistribute it and/or             |
// | modify it under the terms of the GNU General Public License               |
// | as published by the Free Software Foundation; either version 2            |
// | of the License, or (at your option) any later version.                    |
// |                                                                           |
// | This program is distributed in the hope that it will be useful,           |
// | but WITHOUT ANY WARRANTY; without even the implied warranty of            |
// | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
// | GNU General Public License for more details.                              |
// |                                                                           |
// | You should have received a copy of the GNU General Public License         |
// | along with this program; if not, write to the Free Software Foundation,   |
// | Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           |
// |                                                                           |
// +---------------------------------------------------------------------------+

/**
* @package Menu
*/

if (strpos(strtolower($_SERVER['PHP_SELF']), 'functions.inc') !== false) {
    die ('This file can not be used on its own.');
}

/**
* Menu default settings
*
* Initial Installation Defaults used when loading the online configuration
* records.  These settings are only used during the initial installation
* and not referenced any more once the plugin is installed
*/
global $_MENU_DEFAULT;
$_MENU_DEFAULT = array();

// This is the default for 'samplesetting1'
$_MENU_DEFAULT['samplesetting1'] = true;

// This is the default for 'samplesetting2'
$_MENU_DEFAULT['samplesetting2'] = 1;

/**
* Initialize Menu plugin configuration
*
* Creates the database entries for the configuation if they don't already
* exist.  Initial values will be taken from $_MENU_DEFAULT.
*
* @return   boolean     TRUE: success; FALSE: an error occurred
*/
function plugin_initconfig_menu()
{
    global $_MENU_CONF, $_MENU_DEFAULT;

    if (is_array($_MENU_CONF) && (count($_MENU_CONF) > 1)) {
        $_MENU_DEFAULT = array_merge($_MENU_DEFAULT, $_MENU_CONF);
    }

    $me = 'menu';

    $c = config::get_instance();
    if (!$c->group_exists('menu')) {
        $c->add('sg_main', NULL, 'subgroup', 0, 0, NULL, 0, true, $me, 0);
        $c->add('tab_main', NULL, 'tab', 0, 0, NULL, 0, true, $me, 0);
        $c->add('fs_main', NULL, 'fieldset', 0, 0, NULL, 0, true, $me, 0);
        // The below two lines add two settings to Geeklog's config UI
        $c->add('samplesetting1', $_MENU_DEFAULT['samplesetting1'], 'select',0, 0, 1, 10, true, $me, 0); // This adds a drop-down box
        $c->add('samplesetting2', $_MENU_DEFAULT['samplesetting2'], 'text', 0, 0, null, 20, true, $me, 0); // This adds a text input field
    }

    return true;
}
?>

