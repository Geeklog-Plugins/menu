<?php

/* Reminder: always indent with 4 spaces (no tabs). */
// +---------------------------------------------------------------------------+
// | Menu Plugin 1.2.5                                                         |
// +---------------------------------------------------------------------------+
// | functions.inc                                                             |
// |                                                                           |
// | This file does two things: 1) it implements the necessary Geeklog Plugin  |
// | API methods and 2) implements all the common code needed by this plugin.  |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2012-2014 by the following authors:                         |
// |                                                                           |
// | Authors: Ben - ben AT geeklog DOT fr                                      |
// |                                                                           |
// | Based on the original Sitetailor Plugin                                   |
// | Copyright (C) 2008-2011 by the following authors:                         |
// |                                                                           |
// | Mark R. Evans - mark AT glfusion DOT org                                  | 
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
 * Language file include
 */
$plugin_path = $_CONF['path'] . 'plugins/menu/';
$langfile = $plugin_path . 'language/' . $_CONF['language'] . '.php';

if (file_exists($langfile)) {
    require_once $langfile;
} else {
    require_once $plugin_path . 'language/english.php';
}
/**
* Check and see if we need to load the plugin configuration
*/ 
if (!isset($_MENU_CONF)) {
    require_once $_CONF['path_system'] . 'classes/config.class.php';
    
    $menu_config = config::get_instance();
    $_MENU_CONF = $menu_config->get_config('menu');
	require_once __DIR__ . '/autoinstall.php';
	$info = plugin_autoinstall_menu('menu');
	$_MENU_CONF['pi_version'] = $info['info']['pi_version'];
	unset($info);
}
/**
 * Menu plugin table(s)
 */
$_TABLES['menu'] = $_DB_table_prefix . 'menu';
$_TABLES['menu_config'] = $_DB_table_prefix . 'menu_config';
$_TABLES['menu_elements']= $_DB_table_prefix . 'menu_elements';


if (VERSION >= '2.0.0') {
    require_once $_CONF['path'].'plugins/menu/classes/classMenuElement2.php';
} else {
    require_once $_CONF['path'].'plugins/menu/classes/classMenuElement.php';
}

// +---------------------------------------------------------------------------+
// | Geeklog Plugin API Implementation                                         |
// +---------------------------------------------------------------------------+


/**
* Implements a [menu:] autotag.
*
* @param    string  $op         operation to perform
* @param    string  $content    item (e.g. story text), including the autotag
* @param    array   $autotag    parameters used in the autotag
* @param    mixed               tag names (for $op='tagname') or formatted content
*
*/
function plugin_autotags_menu($op,$content='',$autotag='') {

    global $Menus, $_TABLES, $_CONF;

    if ($op == 'tagname' ) {
        return array('menu');
    } else if ($op == 'parse') {
        /*
         * Check to see if the menu name has spaces, if so, concatenate into a single parm
         */
        if ( isset($autotag['parm2']) ) {
            $autotag['parm1'] .= ' ' . $autotag['parm2'];
        }
        switch( $autotag['tag'] ) {
            case 'menu' :
                $menu = '';
                $menuID = trim($autotag['parm1']);
                $id = DB_getItem($_TABLES['menu'],'id','menu_name="'.$menuID.'"');
                if ( $id > 0 ) {
                    if ( $Menus[$id]['menu_type'] == 1 ) {
                        // Horizontal menu
						$menu = MENU_getMenu($menuID,"gl_menu","gl_menu",'',"parent");
                    } else if ( $Menus[$id]['menu_type'] == 2 ) {
                        
						$menu = MENU_getMenu($menuID,'st-fmenu','','','','st-f-last');
                    } else if ( $Menus[$id]['menu_type'] == 3 ) {
					    // Block menu
						$menu = phpblock_getMenu('',trim($autotag['parm1']));
					} else if ( $Menus[$id]['menu_type'] == 4 ) {
					    // Block menu
						$menu = MENU_getMenu($menuID,'st-vmenu','','','');
					} 
					
                    $content = str_replace($autotag['tagstr'],$menu,$content);
                } else {
                    $content = str_replace($autotag['tagstr'],$menu,$content);
                }
                break;
        }
        return $content;
    }
}


/**
* Return plugin entry for "Command and Control" (moderation.php)
*
* @return  array  Array containing (plugin name, admin url, url of plugin icon)
*
*/
function plugin_cclabel_menu()
{
    global $_CONF, $LANG_MENU_1;

    $retval = array();
    if (SEC_hasRights('menu.admin')) {
        $retval = array($LANG_MENU_1['plugin_name'],
            $_CONF['site_admin_url'] . '/plugins/menu/index.php',
            plugin_geticon_menu());
    }

    return $retval;
}

/**
* Return plugin entry for the Admins Only block
*
* @return array Array containing (plugin name, plugin admin url, # of plugin items or '')
*/
function plugin_getadminoption_menu()
{
    global $_CONF, $LANG_MENU_1;

    if (SEC_hasRights('menu.admin')) {
        return array($LANG_MENU_1['plugin_name'],
            $_CONF['site_admin_url'] . '/plugins/menu/index.php', 0);
    }
}

/**
* Returns the URL of the plugin's icon
*
* @return   string      URL of the icon
*
*/
function plugin_geticon_menu()
{
    global $_CONF;

    // popular alternative location:
    // return $_CONF['site_url'] . '/menu/images/menu.png';

    return $_CONF['site_admin_url'] . '/plugins/menu/images/menu.png';
}

/**
 * Returns the current plugin code version
 *
 * @return string    plugin version
 */
function plugin_chkVersion_menu()
{
    global $_CONF;

    require_once $_CONF['path'] . 'plugins/menu/autoinstall.php';

    $inst_parms = plugin_autoinstall_menu('menu');

    return $inst_parms['info']['pi_version'];
}

/**
* Update the plugin
*
* @return   int     Number of message to display or true for success
*
*/
function plugin_upgrade_menu()
{
    global $_CONF, $_TABLES;

    $installed_version = DB_getItem($_TABLES['plugins'], 'pi_version',
                                    "pi_name = 'menu'");
    $code_version = plugin_chkVersion_menu();
    if ($installed_version == $code_version) {
        // nothing to do
        return true;
    }

    require_once $_CONF['path'] . 'plugins/menu/autoinstall.php';

    if (! plugin_compatible_with_this_version_menu('menu')) {
        return 3002;
    }

    // other update code goes here
	
	switch( $currentVersion ) {
	    
		case "0.9" :
		    
			// Create menu folder 
			if (!is_dir($_CONF['path_images'] . 'menu')) {
				@mkdir($_CONF['path_images'] . 'menu');
			}
			// Create cache folder 
			if (!is_dir($_CONF['path_data'] . 'menu')) {
				@mkdir($_CONF['path_data'] . 'menu');
				@mkdir($_CONF['path_data'] . 'menu/cache');
			}
	
		case "1.0" :
		    // Create new cache folder 
			if (!is_dir($_CONF['path_data'] . 'menu_data')) {
				@mkdir($_CONF['path_data'] . 'menu_data');
				@mkdir($_CONF['path_data'] . 'menu_data/cache');
				@mkdir($_CONF['path_data'] . 'menu_data/css');
            }
		case "1.1" :
		case "1.2" :
		case "1.2.1" :
		case "1.2.2" :
		case "1.2.3" :
		case "1.2.4" :
		case "1.2.5" :
		case "1.2.6" :
		
		
		default :
		    MENU_CACHE_cleanup_plugin('');
	
	}

    // update plugin version number
    $inst_parms = plugin_autoinstall_menu('menu');
    $pi_gl_version = $inst_parms['info']['pi_gl_version'];

    DB_query("UPDATE {$_TABLES['plugins']} SET pi_version = '$code_version', pi_gl_version = '$pi_gl_version' WHERE pi_name = 'menu'");
	/* This code is for statistics ONLY */
    $message =  'Completed menu plugin upgrade: ' .date('m d Y',time()) . "   AT " . date('H:i', time()) . "\n";
    $message .= 'Site: ' . $_CONF['site_url'] . ' and Sitename: ' . $_CONF['site_name'] . "\n";
    $pi_version = DB_getItem($_TABLES['plugins'], 'pi_version', "pi_name = 'menu'");
    COM_mail("ben@geeklog.fr","Menu Version:$pi_version Upgrade successfull",$message);

    return true;
}

/**
* Automatic uninstall function for plugins
*
* This code is automatically uninstalling the plugin.
* It passes an array to the core code function that removes
* tables, groups, features and php blocks from the tables.
* Additionally, this code can perform special actions that cannot be
* foreseen by the core code (interactions with other plugins for example)
*
* @return   array   Plugin information
*
*/
function plugin_autouninstall_menu()
{
    $out = array (
        /* give the name of the tables, without $_TABLES[] */
        'tables' => array('menu','menu_config','menu_elements'),
        /* give the full name of the group, as in the db */
        'groups' => array('Menu Admin'),
        /* give the full name of the feature, as in the db */
        'features' => array('menu.admin'),
        /* give the full name of the block, including 'phpblock_', etc */
        'php_blocks' => array(),
        /* give all vars with their name */
        'vars' => array()
    );

    return $out;
}

function plugin_getheadercode_menu() {
    global $_CONF, $Menus, $_SCRIPTS;
	
	//slicknav
	$_SCRIPTS->setCSSFile('menu_slicknav', '/menu/css/slicknav.css');
	$_SCRIPTS->setJavaScriptFile('slicknav', '/menu/js/jquery.slicknav.js');

    $menu_header = '';
    $this_dir = basename( dirname( $_SERVER['SCRIPT_NAME'] ) );
	if ( $this_dir == 'menu' ) {
        $menu_header .= '<meta http-equiv="cache-control" content="no-cache" />' . LB;
        $menu_header .= '<meta http-equiv="pragma" content="no-cache" />' . LB;
    }

    if ( is_array($Menus) ) {
        foreach ($Menus AS $menu) {
            if ( $menu['active'] == 1 ) {
                $cacheInstance = 'menu_css_' . $menu['menu_id'] . '__' . $_CONF['theme'];
                $retval = MENU_CACHE_check_instance($cacheInstance, 0);
				
				//Custom CSS
				$custom_css = $_CONF['path_data'] . 'menu_data/css/gl_menu' . $menu['menu_id'] . '.css';
				if(file_exists($custom_css)) {
					$custom_ccs_code = LB . file_get_contents($custom_css);
				} else {
					$custom_ccs_code = '';
				}

                if ( $retval ) {
                    $css_minify = MENU_compress( $retval . $custom_ccs_code);
					$menu_header .= LB. '<style type="text/css">'. $css_minify . '</style>' . LB;
                } else {
                    $ms = new Template( $_CONF['path'].'plugins/menu/templates/' );
					$ms->set_var('under',50);
					$ms->set_var('over',99);
                    switch ($menu['menu_type']) {
                        case 1 :    // horizontal with multi-level
                            $stylefile = 'gl_horizontal-cascading.thtml';
                            break;
                        case 2 :    // horizontal with no cascading menus
                            $stylefile = 'gl_horizontal-simple.thtml';
                            break;
                        case 3 :
                            $stylefile = 'gl_vertical-cascading.thtml';
							$ms->set_var('under',25);
                            break;
                        case 4 :
                            $stylefile = 'gl_vertical-simple.thtml';
							$ms->set_var('under',25);
                            break;
                        default :
                            $stylefile = 'gl_horizontal-cascading.thtml';
                            break;
                    }
                    $ms->set_file( array(
                        'style'        => $stylefile,
                    ));
                    $ms->set_var('menu_id',$menu['menu_id']);
					
					$ms->set_var('site_url',$_CONF['site_url']);
                    if ( is_array($menu['config']) ) {
                        foreach ($menu['config'] AS $name => $value ) {
                            if ( $name == 'use_images' && $value == 0 ) {
								$ms->set_var('url1','');
								$ms->set_var('url2','');
								continue;
                            } else if ( $name == 'use_images' && $value == 1 ){
							    $url1 = "url({$_CONF['site_url']}/images/menu/{menu_bg_filename}) repeat-x";
								$url2 = "url({$_CONF['site_url']}/images/menu/{menu_hover_filename}) repeat-x";
								$ms->set_var('url1',$url1);
								$ms->set_var('url2',$url2);
								continue;
							}
                            $ms->set_var($name,$value);
                        }
                    }
                    if ( $menu['config']['menu_alignment'] == 1 ) {
                        $ms->set_var('alignment','left');
                    } else {
                        $ms->set_var('alignment','right');
                    }

                    $ms->parse ('output', 'style');
                    $fCSS = $ms->finish ($ms->get_var('output'));
					
					//slicknav css
					if ($menu['menu_type'] == 1) {
					    $slicknave_css = " .slicknav_menu { display:none; } @media screen and (max-width: 750px) { #gl_menu" . $menu['menu_id'] . " { display:none; } .slicknav_menu { display:block; } }";
					} else {
					     $slicknave_css = '';
					}
						
					$fCSS = $fCSS . $slicknave_css;
					
                    MENU_CACHE_create_instance($cacheInstance, $fCSS, 0);
					$css_minify = MENU_compress( $fCSS . $custom_ccs_code);
                    $menu_header .= LB . '<style type="text/css">' . $css_minify . '</style>' . LB;
                }
				
				$js = LB . "jQuery(document).ready(function() {
					jQuery('#gl_menu" . $menu['menu_id'] . "').slicknav(({
						label: '{$_CONF['site_name']}',
						allowParentLinks: true,
					}));
					jQuery( '.slicknav_nav .gl_menu" . $menu['menu_id'] . "' ).removeClass( 'gl_menu" . $menu['menu_id'] . "' );
				});" . LB;
				
				/*
				Available Options for jQuery slicknav plugin
				Property 	Default 	Description
				label 	"MENU" 	Label for menu button. Use an empty string for no label.
				duplicate 	true 	If true, a copy of the menu element is made for the mobile menu. This allows for separate functionality for both mobile and non-mobile versions.
				duration 	200 	The duration of the sliding animation.
				easingOpen 	"swing" 	Easing used for open animations. "Swing" and "Linear" are available with jQuery. More easing functions are available with the use of plug-ins, such as jQuery UI.
				easingClose 	"swing" 	Easing used for close animations. "Swing" and "Linear" are available with jQuery. More easing functions are available with the use of plug-ins, such as jQuery UI.
				closedSymbol 	"&#9658;" 	Character after collapsed parents..
				openedSymbol 	"&#9660;" 	Character after expanded parents.
				prependTo 	"body" 	Element, jQuery object, or jQuery selector string for the element to prepend the mobile menu to.
				parentTag 	"a" 	Element type for parent menu items. Anchor tag is recommended for accessibility.
				closeOnClick 	false 	Close menu when a link is clicked. Useful when navigating within a single page.
				allowParentLinks 	false 	Allow clickable links as parent elements. 
				*/
            }
        }
    }
    
	//slicknav js   
    $_SCRIPTS->setJavaScriptLibrary('jquery');
    $_SCRIPTS->setJavaScript($js, true);
	
	//$menu_header .= "<script defer type=\"text/javascript\" src=\"" . $_CONF['site_url'] . '/menu/js/menu_ie6vertmenu.js"' . XHTML . '>' . LB;
    return $menu_header;
}

function MENU_initMenu() {
    global $Menus,$_GROUPS, $_MENU_CONF, $_TABLES, $_USER;

    $mbadmin = SEC_hasRights('menu.admin');
    $root    = SEC_inGroup('Root');

    if (empty($_USER['uid']) ) {
        $uid = 1;
    } else {
        $uid = $_USER['uid'];
    }

    $result = DB_query("SELECT * FROM {$_TABLES['menu']}",1);
    while ( $menu = DB_fetchArray($result) ) {
        $menuID = $menu['id'];
        $Menus[$menu['id']]['menu_name']   = $menu['menu_name'];
        $Menus[$menu['id']]['menu_id']     = $menu['id'];
        $Menus[$menu['id']]['active']      = $menu['menu_active'];
        $Menus[$menu['id']]['menu_type']   = $menu['menu_type'];
        $Menus[$menu['id']]['group_id']    = $menu['group_id'];

        if ($mbadmin || $root) {
            $Menus[$menu['id']]['menu_perm'] = 3;
        } else {
            if ( $menu['group_id'] == 998 ) {
                if( COM_isAnonUser() ) {
                    $Menus[$menu['id']]['menu_perm'] = 3;
                } else {
                    $Menus[$menu['id']]['menu_perm'] =  0;
                }
            } else {
                if ( in_array( $menu['group_id'], $_GROUPS ) ) {
                    $Menus[$menu['id']]['menu_perm'] =  3;
                }
            }
        }

        // retrieve configuration options for this menu...
        $cfgResult = DB_query("SELECT * FROM {$_TABLES['menu_config']} WHERE menu_id=".intval($menu['id']));
        while ($cfgRow = DB_fetchArray($cfgResult)) {
            $Menus[$menu['id']]['config'][$cfgRow['conf_name']] = $cfgRow['conf_value'];
        }

        $sql = "SELECT * FROM {$_TABLES['menu_elements']} WHERE menu_id=".intval($menuID)." ORDER BY element_order ASC";
        $elementResult      = DB_query( $sql, 1);
        $element            = new mbElement();
        $element->id        = 0;
        $element->menu_id   = $menuID;
        $element->label     = 'Top Level Menu';
        $element->type      = -1;
        $element->pid       = 0;
        $element->order     = 0;
        $element->url       = '';
        $element->owner_id  = $mbadmin;
        $element->group_id  = $root;
        if ( $mbadmin ) {
            $element->access = 3;
        }
        $Menus[$menuID]['elements'][0] = $element;

        while ($A = DB_fetchArray($elementResult) ) {
            $element  = new mbElement();
            $element->constructor($A,$mbadmin,$root,$_GROUPS);
            if ( $element->access > 0 ) {
                $Menus[$menuID]['elements'][$element->id] = $element;
            }
        }
    }

    if ( is_array($Menus) ) {
        foreach( $Menus as $name => $menu ) {
            foreach( $Menus[$name]['elements'] as $id => $element) {
                if ($id != 0 && isset($Menus[$name]['elements'][$element->pid]->id) ) {
                    $Menus[$name]['elements'][$element->pid]->setChild($id);
                }
            }
        }
    }
}

/*
 * This function will return the HTML (using <ul><li></ul>) structure
 */


function MENU_getMenu($name='navigation',$wrapper='',$ulclass='',$liclass='',$parentclass='',$lastclass='',$selected='',$noid=0) {
    global $Menus, $_CONF;

    $optionHash = md5($wrapper.$ulclass.$liclass.$parentclass.$lastclass.$selected);

    $menuID = '';
    $lang = COM_getLanguageId();

    if (!empty($lang)) {
        $mlname = $name . '_'.$lang;

        $cacheInstance = 'menu_' . $mlname . '_' . MENU_CACHE_security_hash() . '_' . $optionHash . '__' . $_CONF['theme'];

        $retval = MENU_CACHE_check_instance($cacheInstance, 0);
        if ( $retval && $noid == 0) {
            return $retval;
        }
        $retval = '';
        if ( is_array($Menus) ) {
            foreach($Menus AS $id) {
                if ( $id['menu_name'] == $mlname ) {
                    $menuID = $id['menu_id'];
                    break;
                }
            }
        }
    }
    if ( $menuID == '' ) {
        if ( !empty($retval) && $noid == 0) {
            return $retval;
        }
        $retval = '';
        $menuID = '';
        if ( is_array($Menus) ) {
            foreach($Menus AS $id) {
                if ( strcasecmp(trim($id['menu_name']), trim($name)) == 0 ) {
                    $menuID = $id['menu_id'];
                    break;
                }
            }
        }
    }
    if ( $menuID != '' && $Menus[$menuID]['active'] == 1 && $Menus[$menuID]['menu_perm'] == 3) {
        if ( $Menus[$menuID]['menu_type'] == 1 ) {
            $retval .= '<div id="gl_menu'.($noid == 0 ? $menuID : '').'">'. LB;
        }
        if ( $wrapper != '' ) {
            $retval .= '<div class="'.$wrapper.($noid == 0 ? $menuID : '').'">' . LB;
        }
        $retval .= $Menus[$menuID]['elements'][0]->showTree(0,$ulclass,$liclass,$parentclass,$lastclass,$selected);
        if ( $wrapper != '' ) {
            $retval .= '</div>' . LB;
        }
        if ( $Menus[$menuID]['menu_type'] == 1 ) {
            $retval .= '</div>'. LB;
        }
    } else {
        return '';
    }

    return $retval;
}

function phpblock_getMenu($arg1,$arg2) {
    global $Menus, $_GROUPS, $_CONF;

    if ( is_array($Menus) ) {
        foreach($Menus AS $id) {
            if ( $id['menu_name'] == $arg2 ) {
                $menuID = $id['menu_id'];
                break;
            }
        }
    }
    if ( $Menus[$menuID]['active'] != 1 || $Menus[$menuID]['menu_perm'] == 0) {
        return;
    }

    if ( $Menus[$menuID]['menu_type'] == 4 ) {
        $menu = MENU_getMenu($arg2,'st-vmenu','','','');
        if ( $menu != '' ) {
            $retval = $menu;
        } else {
            $retval = '';
        }
        return $retval;
    }
    if ( $Menus[$menuID]['config']['menu_alignment'] == 1 ) {
        $parent = 'parent-l';
        $class  = 'gl_menu-vert-l';
    } else {
        $parent = 'parent-r';
        $class  = 'gl_menu-vert-r';
    }
    $menu = MENU_getMenu($arg2,$class,'','',$parent);
    if ( $menu != '' ) {
        $retval = '<div id="'.$class.$menuID.'">';
        $retval .= $menu;
        $retval .= '</div>';
    } else {
        $retval = '';
    }

    return $retval;
}

function MENU_getImageFile($image) {
	global $_CONF;

    return $_CONF['site_admin_url'] . '/plugins/menu/images/' . $image;
}

function MENU_PLG_getMenuItems()
{
    global $_PLUGINS;

    $menu = array();
    foreach ($_PLUGINS as $pi_name) {
        $function = 'plugin_getmenuitems_' . $pi_name;
        if (function_exists($function)) {
            $menuitems = $function();
            if (is_array ($menuitems)) {
                $url = current($menuitems);
                $label = key($menuitems);
                $mbmenu[$pi_name] = $url;
                $menu = array_merge ($menu, $mbmenu );
            }
        }
    }
    return $menu;
}

MENU_initMenu();

/**
* Set template variables
*
* @param    string  $templatename   name of template, e.g. 'header'
* @param    ref     $template       reference of actual template
* @return   void
*
*/
function plugin_templatesetvars_menu ($templatename, &$template)
{
    // Prevent from execution immediately behind the uninstallation of this plugin
    if (!DB_checkTableExists('menu')) return;
	$menu = MENU_getMenu('navigation',"gl_menu","gl_menu",'',"parent");
    $template->set_var('header_navigation',$menu);
	$menu_footer = MENU_getMenu('footer',"st-fmenu","st-fmenu",'',"parent");
	$template->set_var('menu_footer',$menu_footer);
	$template->set_var('breadcrumbs', '');
}

$MENU_TEMPLATE_OPTIONS = array(
    'path_cache'    => $_CONF['path_data'] . 'menu_data/cache/',   // location of template cache
    'path_prefixes' => array(                               // used to strip directories off file names. Order is important here.
                        $_CONF['path_themes'],  // this is not path_layout. When stripping directories, you want files in different themes to end up in different directories.
                        $_CONF['path'],
                        '/'                     // this entry must always exist and must always be last
                       ),
    'incl_phpself_header' => true,          // set this to true if your template cache exists within your web server's docroot.
    'cache_by_language' => true,            // create cache directories for each language. Takes extra space but moves all $LANG variable text directly into the cached file
    'default_vars' => array(                                // list of vars found in all templates.
                        'site_url' => $_CONF['site_url'],
                        'site_admin_url' => $_CONF['site_admin_url'],
                        'layout_url' => $_CONF['layout_url'],
                        'xhtml' => (defined('XHTML') ? XHTML : ' /'),
                      ),
    'hook' => array(),
);
/******************************************************************************
 * Function used to traverse directory tree when cleaning cache
 *
 * usage: MENU_cache_clean_directories($plugin);
 *
 * @param  $path            Directory path being cleaned
 * @param  $needle          String matched against cache filenames
 * @access private
 * @return void
 *
 */
function MENU_cache_clean_directories($path, $needle = '', $since = 0)
{
    if ($dir = @opendir($path)) {
        while (false !== ($entry = readdir($dir))) {
            if ($entry == '.' || $entry == '..' || $entry == '.svn' || is_link($entry)) {
            } elseif (is_dir($path . '/' . $entry)) {
                MENU_cache_clean_directories($path . '/' . $entry, $needle);
                @rmdir($path . '/' . $entry);
            } elseif (empty($needle) || strpos($entry, $needle) !== false) {
                if (!$since || @filectime($path . '/' . $entry) <= $since) {
                    @unlink($path . '/' . $entry);
                }
            }
        }
        @closedir($dir);
    }
}

/******************************************************************************
 * Removes all cached files associated with a plugin.
 *
 * usage: MENU_CACHE_cleanup_plugin($plugin);
 *
 * @param  $plugin          String containing the plugin's name
 * @access public
 * @return void
 *
 */
function MENU_CACHE_cleanup_plugin($plugin)
{
    global $MENU_TEMPLATE_OPTIONS;

    if (!empty($plugin)) {
        $plugin = str_replace(array('..', '/', '\\'), '', $plugin);
        $plugin = '__' . $plugin . '__';
    }
    $path_cache = substr($MENU_TEMPLATE_OPTIONS['path_cache'], 0, -1);
    MENU_cache_clean_directories($path_cache, $plugin);
}

/******************************************************************************
 * Deletes an instance of the specified instance identifier
 *
 * usage: MENU_CACHE_remove_instance($iid, $glob);
 *
 * @param  $iid            A globally unique instance identifier.
 * @access public
 * @return void
 * @see    check_instance, create_instance
 *
 */
function MENU_CACHE_remove_instance($iid)
{
    global $MENU_TEMPLATE_OPTIONS;

    $iid = str_replace(array('..', '/', '\\', ':'), '', $iid);
    $iid = str_replace('-','_',$iid);
    $path_cache = substr($MENU_TEMPLATE_OPTIONS['path_cache'], 0, -1);
    MENU_CACHE_clean_directories($path_cache, 'instance__'.$iid);
}


/******************************************************************************
 * Creates a cached copy of the data passed.
 *
 * usage: MENU_CACHE_create_instance($iid, $data, $bypass_lang);
 *
 * @param  $iid            A globally unique instance identifier.
 * @param  $data           The data to cache
 * @param  $bypass_lang    If true, the cached data is not instanced by language
 * @access public
 * @return void
 * @see    MENU_CACHE_check_instance, MENU_CACHE_remove_instance
 *
 */
function MENU_CACHE_create_instance($iid, $data, $bypass_lang = false)
{
    global $MENU_TEMPLATE_OPTIONS, $_CONF;

    if ($MENU_TEMPLATE_OPTIONS['cache_by_language']) {
        if (!is_dir($MENU_TEMPLATE_OPTIONS['path_cache'] . $_CONF['language'])) {
            @mkdir($MENU_TEMPLATE_OPTIONS['path_cache'] . $_CONF['language']);
            @touch($MENU_TEMPLATE_OPTIONS['path_cache'] . $_CONF['language'] . '/index.html');
        }
    }

    $filename = MENU_CACHE_instance_filename($iid, $bypass_lang);
    @file_put_contents($filename, $data);
}

/******************************************************************************
 * Finds a cached copy of the referenced data.
 *
 * usage: $data = MENU_CACHE_check_instance($iid, $bypass_lang)
 *        if (!$data === false) {
 *            // generate the data
 *            $data = 'stuff';
 *            MENU_CACHE_create_instance($iid, $data, $bypass_lang);
 *        }
 *        // use the data
 *
 * The caching functions only work with strings. If you want to store structures
 * you must serialize/unserialize the data yourself:
 *
 *      $data = MENU_CACHE_check_instance($iid);
 *      if ($data === false) {
 *          $data = new SomeObj();
 *          MENU_CACHE_create_instance($iid, serialize($data));
 *      } else {
 *          $data = unserialize($data);
 *      }
 *      // use the object
 *
 * @param  $iid            A globally unique instance identifier.
 * @param  $bypass_lang    If true, the cached data is not instanced by language
 * @access public
 * @return the data string or false is there is no such instance
 * @see    MENU_CACHE_check_instance, MENU_CACHE_remove_instance
 *
 */
function MENU_CACHE_check_instance($iid, $bypass_lang = false)
{
    global $_CONF;

    $filename = MENU_CACHE_instance_filename($iid, $bypass_lang);
    if (file_exists($filename)) {
        $str = @file_get_contents($filename);
        return $str === FALSE ? false : $str;
    }
    return false;
}

/******************************************************************************
 * Returns the time when the referenced instance was generated.
 *
 * usage: $time = MENU_CACHE_get_instance_update($iid, $bypass_lang = false)
 *
 * @param  $iid            A globally unique instance identifier.
 * @param  $bypass_lang    If true, the cached data is not instanced by language
 * @access public
 * @return unix_timestamp of when the instance was generated or false
 * @see    MENU_CACHE_check_instance, MENU_CACHE_remove_instance
 *
 */
function MENU_CACHE_get_instance_update($iid, $bypass_lang = false)
{
    global $_CONF;

    $filename = MENU_CACHE_instance_filename($iid, $bypass_lang);
    return @filemtime($filename);

}

/******************************************************************************
 * Generates a full path to the instance file. Should really only be used
 * internally but there are probably reasons to use it externally.
 *
 * usage: $time = MENU_CACHE_instance_filename($iid, $bypass_lang = false)
 *
 * @param  $iid            A globally unique instance identifier.
 * @param  $bypass_lang    If true, the cached data is not instanced by language
 * @access public
 * @return unix_timestamp of when the instance was generated or false
 * @see    MENU_CACHE_create_instance, MENU_CACHE_check_instance, MENU_CACHE_remove_instance
 *
 */
function MENU_CACHE_instance_filename($iid,$bypass_lang = false)
{
    global $MENU_TEMPLATE_OPTIONS, $_CONF;

    $path_cache = $MENU_TEMPLATE_OPTIONS['path_cache'];
    if (!$bypass_lang && $MENU_TEMPLATE_OPTIONS['cache_by_language']) {
        $path_cache .= $_CONF['language'] . '/';
    }
    $iid = COM_sanitizeFilename($iid, true);
    $filename = $path_cache.'instance__'.$iid.'.php';

    return $filename;
}

/******************************************************************************
 * Generates a hash based on the current user's secutiry profile.
 *
 * Currently that is just a list of groups the user is a member of but if
 * additional data is found to be necessary for creating a unique security
 * profile, this centralized function would be the place for it.
 *
 * usage: $hash = MENU_CACHE_security_hash()
 *        $instance = "somedata__$someid__$hash";
 *        MENU_CACHE_create_instance($instance, $thedata);
 *
 * @access public
 * @return a string uniquely identifying the user's security profile
 *
 */
function MENU_CACHE_security_hash()
{
    global $_GROUPS, $_USER;

    static $hash = NULL;

    if (empty($hash)) {
        $groups = implode(',',$_GROUPS);
        $hash = strtolower(md5($groups));
        if ( !empty($_USER['tzid']) ) {
            $hash .= 'tz'.md5($_USER['tzid']);
        }
    }
    return $hash;

}

function MENU_compress($buffer) {
    
	/* remove comments */
    $buffer = preg_replace('!/\*[^*]*\*+([^/][^*]*\*+)*/!', '', $buffer);
    /* remove tabs, spaces, newlines, etc. */
    $buffer = str_replace(array("\r\n", "\r", "\n"), '', $buffer);
    return $buffer;
}
?>
