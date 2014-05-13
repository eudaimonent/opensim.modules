<?php

if (!defined('ENV_READED_INTERFACE')) include_once('../include/env_interface.php');


if (defined('CMS_DB_HOST')) 
{
	$DB_HOST = 	   CMS_DB_HOST;
	$DB_NAME = 	   CMS_DB_NAME;
	$DB_USER = 	   CMS_DB_USER;
	$DB_PASSWORD = CMS_DB_PASS;
	$DB_MYSQLI =   CMS_DB_MYSQLI;
}
else if (defined('OPENSIM_DB_HOST'))
{
    $DB_HOST =     OPENSIM_DB_HOST;
    $DB_NAME =     OPENSIM_DB_NAME;
    $DB_USER =     OPENSIM_DB_USER;
    $DB_PASSWORD = OPENSIM_DB_PASS;
    $DB_MYSQLI =   OPENSIM_DB_MYSQLI;
}
else
{
	// if you donot have env_interface.php, please set DB information by manual.
    $DB_HOST =     'localhost';
    $DB_NAME =     '';
    $DB_USER =     '';
    $DB_PASSWORD = '';
    $DB_MYSQLI =   false;
}



if (!defined('PROFILE_CLASSIFIEDS_TBL'))
{
	$GLOBALS['xmlrpc_internalencoding'] = 'UTF-8';

	define('PROFILE_CLASSIFIEDS_TBL',  'classifieds');
	define('PROFILE_USERNOTES_TBL',    'usernotes');
	define('PROFILE_USERPICKS_TBL',    'userpicks');
	define('PROFILE_USERPROFILE_TBL',  'userprofile');
	define('PROFILE_USERSETTINGS_TBL', 'usersettings');
}

?>
