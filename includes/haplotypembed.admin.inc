<?php
/**
 * Administration for this module.
 */

 /**
 * Provide a landing page for this module by listing all haplotype maps (files).
 */

 // list all maps (files)
  function haplotypemap_embed_admin_list() {
     $output = '';

     $output .= '<p>This module provides a form for users to utilize haplotype file visulation module haplotype map.:</p>';

     $output .= l('Add', 'admin/tripal/extension/haplotype-map/add');

     $table = array(
       'header' => array('Human-readable Name', 'Number of Germplasm', 'Germplasm in Default File'),
       'rows' => array(),
     );

     $results = db_query('SELECT * FROM {haplotypembed_files}');
     foreach ($results as $r) {
       // count number of germplasm first

       $table['rows'][] = array(
         'name' => $r->human_readable_name,
         'total_number_germplasm' => count(explode("\n", $r->all_germplasm)),
         'file' => join(";", explode("\n", $r->default_germplasm)),
         l('edit', 'admin/tripal/extension/haplotype-map/edit/'.$r->haplotypembed_file_id) .' | '
           . l('delete', 'admin/tripal/extension/haplotype-map/delete/'.$r->haplotypembed_file_id) .' | '
           . l('access', 'admin/tripal/extension/haplotype-map/access/'.$r->haplotypembed_file_id)
       );
     }
     // If there are no files yet, then tell the admin this.
     if (empty($table['rows'])) {
       $table['rows'][] = array(array('data' => 'None.', 'colspan' => 4));
     }

     $output .= theme('table', $table);

     return $output;
  }


  function haplotypemap_embed_admin_file_form($form, &$form_state) {

    $obj = NULL;

    if (isset($form_state['build_info']['args'][1])) {
      $obj = db_query('SELECT * FROM {haplotypembed_files} WHERE haplotypembed_file_id=:id',
        array(':id' => $form_state['build_info']['args'][1]))->fetchObject();
    }

    $form['haplotypembed_file_id'] = array(
      '#type' => 'hidden',
      '#value' => (isset($obj->haplotypembed_file_id)) ? $obj->haplotypembed_file_id : NULL,
    );

    if ($form_state['build_info']['args'][0] == 'delete') {

      $table = array(
        'header' => NULL,
        'rows' => array(
          array(array('data' => 'Haplotype map File', 'header' => TRUE), $obj->file_path),
          array(array('data' => 'Human-readable Name', 'header' => TRUE), $obj->human_readable_name),
          array(array('data' => 'Default File', 'header' => TRUE), $obj->default_file_path),
        ),
      );

      $form['info'] = array(
        '#type' => 'markup',
        '#markup' => '<p>Would you like to delete this haplotype map file?</p>'
          .theme('table', $table)
          .'<p><strong>Description:</strong> '.$obj->description.'</p>'
          .'<p><strong>Germplasm in default filer:</strong> '.$obj->default_germplasm.'</p>'
          .'<p><strong>Germplasm in file:</strong> '.$obj->all_germplasm.'</p>'
      );

      $form['name'] = array(
        '#type' => 'hidden',
        '#value' => $obj->name,
      );

      $form['delete'] = array(
        '#type' => 'submit',
        '#value' => 'Delete',
        '#suffix' => '<span> ' . l('or Cancel', '/admin/tripal/extension/haplotype-map') . '</span>',
      );

      return $form;
    }

    $form['name'] = array(
      '#type' => 'textfield',
      '#title' => 'Human-readable Name',
      '#description' => 'This is the name shown to your users so make sure it is descriptive and uniquely identifies the Haplotype Map file',
      '#default_value' => (isset($obj->human_readable_name)) ? $obj->human_readable_name : '',
    );

    $form['file'] = array(
      '#type' => 'textfield',
      '#title' => 'Haplotype map file',
      '#description' => 'The absolute path to your haplotype map file. This file must already exist.',
      '#default_value' => (isset($obj->file_path)) ? $obj->file_path : '',
    );

    $form['default_file'] = array(
      '#type' => 'textfield',
      '#title' => 'Default haplotype map file ',
      '#description' => 'The absolute path to your filtred default hapletype map file. This file must already exist.',
      '#default_value' => (isset($obj->default_file_path)) ? $obj->default_file_path : '',
    );

    $form['description'] = array(
      '#type' => 'textarea',
      '#title' => 'Description',
      '#description' => 'This should include the information of the file for this haplotype map, also how the file is filtered for deafult file.',
      '#default_value' => (isset($obj->description)) ? $obj->description : '',
    );

    $form['all_germplasm'] = array(
      '#type' => 'textarea',
      '#title' => 'All germplasm',
      '#description' => 'This should include the names of all germplasm for this haplotype map.',
      '#default_value' => (isset($obj->all_germplasm)) ? $obj->all_germplasm : '',
    );

    $form['default_germplasm'] = array(
      '#type' => 'textarea',
      '#title' => 'Germplasm in default file',
      '#description' => 'This should include the names of germplasm in default file of this haplotype map.',
      '#default_value' => (isset($obj->default_germplasm)) ? $obj->default_germplasm : '',
    );

    $op = ucwords($form_state['build_info']['args'][0]);
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => ($op == 'Edit') ? 'Save Changes' : $op,
    );

    return $form;
  }

  function haplotypemap_embed_admin_file_form_validate($form, &$form_state) {

  }

  function haplotypemap_embed_admin_file_form_submit($form, &$form_state) {
    $form_state['rebuild'] = FALSE;
    $form_state['redirect'] = 'admin/tripal/extension/haplotype-map';

    if ($form_state['values']['op'] == 'Delete') {
      db_query('DELETE FROM {haplotypembed_files} WHERE haplotypembed_file_id=:id',
        array(':id' => $form_state['values']['haplotypembed_file_id']));
      db_query('DELETE FROM {haplotypembed_perm} WHERE haplotypembed_file_id=:id',
        array(':id' => $form_state['values']['haplotypembed_file_id']));
      drupal_set_message('Successfully deleted <em>'.$form_state['values']['name'].'</em>');
    }
    else {
      $record = array(
        'file_path' => $form_state['values']['file'],
        'default_file_path' => $form_state['values']['default_file'],
        'description' => str_replace(',','', $form_state['values']['description']),
        'human_readable_name' => $form_state['values']['name'],
        'all_germplasm' => $form_state['values']['all_germplasm'],
        'default_germplasm' => $form_state['values']['default_germplasm'],
      );

      if ($form_state['values']['op'] == 'Add') {
        drupal_write_record('haplotypembed_files', $record);
        $haplotypemap_embed_lib_path = libraries_get_path('haplotype-map', array('absolute' => TRUE)).'/dist/data';
        haplotypemap_create_symlink_when_add_file($form_state['values']['file'], $haplotypemap_embed_lib_path);
        haplotypemap_create_symlink_when_add_file($form_state['values']['default_file'], $haplotypemap_embed_lib_path);
      }
      else {
        $record['haplotypembed_file_id'] = $form_state['values']['haplotypembed_file_id'];
        drupal_write_record('haplotypembed_files', $record, 'haplotypembed_file_id');
      }
    }
    // create soft link to module haplotype-map dist/data for default file
    $path_to_module_data = DRUPAL_ROOT.'/'.libraries_get_path('haplotype-map', $base_path = FALSE).'/dist/data/'.basename($form_state['values']['default_file']);
    shell_exec("ln -s $form_state['values']['default_file'] $path_to_module_data");
  }

  function haplotypemap_embed_admin_file_perm_form($form, &$form_state){
    $obj = NULL;
    if (isset($form_state['build_info']['args'][0])) {
      $obj = db_query('SELECT * FROM {haplotypembed_files} WHERE haplotypembed_file_id=:id',
        array(':id' => $form_state['build_info']['args'][0]))->fetchObject();
    }
    $form['haplotypembed_file_id'] = array(
      '#type' => 'hidden',
      '#value' => (isset($obj->haplotypembed_file_id)) ? $obj->haplotypembed_file_id : NULL,
    );
    $form['info'] = array(
      '#type' => 'markup',
      '#theme' => 'table',
      '#header' => NULL,
      '#rows' => array(
        array(array('data' => 'Haplotype Map File', 'header' => TRUE), $obj->file_path),
        array(array('data' => 'Human-readable Name', 'header' => TRUE), $obj->human_readable_name),
        array(array('data' => 'Default Map File', 'header' => TRUE), $obj->default_file_path),
        array(array('data' => 'Names of All Germplasm', 'header' => TRUE), $obj->all_germplasm),
        array(array('data' => 'Names of Default Germplasm', 'header' => TRUE), $obj->default_germplasm),
        array(array('data' => 'Description', 'header' => TRUE), $obj->description),
      ),
    );

    $form['role'] = array(
    '#type' => 'markup',
    '#tree' => TRUE,
    '#title' => 'Role Permissions',
    '#caption' => 'All users with the following roles have access to <em>'.$obj->human_readable_name.'</em>.',
    '#theme' => 'haplotypembed_admin_form_perm_table',
  );

  $count = 0;
  $resource = db_query('
    SELECT r.rid, r.name
    FROM {haplotypembed_perm} perm
    LEFT JOIN {role} r ON r.rid=perm.rid
    WHERE perm.rid IS NOT NULL AND haplotypembed_file_id=:id',
    array(':id' => $obj->haplotypembed_file_id));
  foreach ($resource as $r) {
    $count++;

    $form['role'][$r->rid]['id'] = array(
      '#type' => 'item',
      '#value' => $r->rid,
    );

    $form['role'][$r->rid]['name'] = array(
      '#type' => 'item',
      '#value' => $r->name,
    );

    $form['role'][$r->rid]['remove_url'] = array(
      '#type' => 'item',
      '#value' => 'admin/tripal/extension/haplotype-map/access/'.$obj->haplotypembed_file_id.'/remove-role/'.$r->rid,
    );

  }
  if (!$count) {
    $form['role']['none']['name'] = array(
      '#type' => 'item',
      '#value' => 'None.',
    );
  }

  $options = array();
  $resource = db_query('SELECT rid, name FROM {role}');
  foreach ($resource as $r) {
    $options[ $r->rid ] = $r->name;
  }
  $form['role']['add']['form_element'] = array(
    '#type' => 'select',
    '#title' => 'Role to give permission to',
    '#options' => $options,
    '#empty_value' => 'None',
  );

  $form['role']['add']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add Role',
  );

  // USER:
  //---------------
  $form['user'] = array(
    '#type' => 'markup',
    '#tree' => TRUE,
    '#title' => 'User Permissions',
    '#caption' => 'The following users have access to <em>'.$obj->name.'</em>.',
    '#theme' => 'haplotypembed_admin_form_perm_table',
  );

  $count = 0;
  $resource = db_query('
    SELECT u.uid, u.name
    FROM {haplotypembed_perm} perm
    LEFT JOIN {users} u ON u.uid=perm.uid
    WHERE perm.uid IS NOT NULL AND haplotypembed_file_id=:id',
    array(':id' => $obj->haplotypembed_file_id));
  foreach ($resource as $r) {
    $count++;

    $form['user'][$r->uid]['id'] = array(
      '#type' => 'item',
      '#value' => $r->uid,
    );

    $form['user'][$r->uid]['name'] = array(
      '#type' => 'item',
      '#value' => $r->name,
    );

    $form['user'][$r->uid]['remove_url'] = array(
      '#type' => 'item',
      '#value' => 'admin/tripal/extension/haplotype-map/access/'.$obj->haplotypembed_file_id.'/remove-user/'.$r->uid,
    );

  }
  if (!$count) {
    $form['user']['none']['name'] = array(
      '#type' => 'item',
      '#value' => 'None.',
    );
  }

  $form['user']['add']['form_element'] = array(
    '#type' => 'textfield',
    '#title' => 'User to give permission to',
    '#autocomplete_path' => 'autocomplete/haplotype-map/user',
  );

  $form['user']['add']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add User',
  );

  return $form;

  }

function haplotypemap_embed_admin_file_perm_form_validate($form, &$form_state){

}

function haplotypemap_embed_admin_file_perm_form_submit($form, &$form_state){
  if ($form_state['clicked_button']['#value'] == 'Add Role') {
    $record = array(
      'haplotypembed_file_id' => $form_state['values']['haplotypembed_file_id'],
      'rid' => $form_state['values']['role']['add']['form_element'],
    );
    drupal_write_record('haplotypembed_perm', $record);
  }
  elseif ($form_state['clicked_button']['#value'] == 'Add User') {
    $uid = db_query('SELECT uid FROM {users} WHERE name=:name',
      array(':name' => $form_state['values']['user']['add']['form_element']))->fetchField();
    if ($uid) {
      $record = array(
        'haplotypembed_file_id' => $form_state['values']['haplotypembed_file_id'],
        'uid' => $uid,
      );
      drupal_write_record('haplotypembed_perm', $record);
    }
  }

}

function haplotypembed_admin_file_perm_remove_callback($haplotypembed_file_id, $action, $id) {

  if ($action == 'remove-role') {

    $num_deleted = db_delete('haplotypembed_perm')
      ->condition('haplotypembed_file_id', $haplotypembed_file_id)
      ->condition('rid', $id)
      ->execute();
    if ($num_deleted) {
      drupal_set_message('Successfully removed permission to role.');
    }

  }
  elseif ($action == 'remove-user') {

    $num_deleted = db_delete('haplotypembed_perm')
      ->condition('haplotypembed_file_id', $haplotypembed_file_id)
      ->condition('uid', $id)
      ->execute();
    if ($num_deleted) {
      drupal_set_message('Successfully removed permission to user.');
    }

  }

  // Redirect to access page.
  drupal_goto('admin/tripal/extension/haplotype-map/access/'.$haplotypembed_file_id);
}

// theme table
function haplotypembed_admin_form_perm_table($vars) {
  $element = $vars['element'];
  $output = '';

  $table = array(
    'caption' => $element['#title'].': '.$element['#caption'],
    'header' => array('Name','Operations'),
    'rows' => array(),
  );

  foreach (element_children($element) as $key) {

    if ($key == 'add') {
      $table['rows'][] = array(
        array('data' => render($element[$key]['form_element']), 'class' => array('name', 'add')),
        array('data' => render($element[$key]['submit']), 'class' => array('op', 'add')),
      );
    }
    else {
      $remove_link = '';
      if (isset($element[$key]['remove_url'])) {
        $remove_link = l('remove', $element[$key]['remove_url']['#value']);
      }

      $table['rows'][] = array(
        array('data' => $element[$key]['name']['#value'], 'class' => array('name')),
        array('data' => $remove_link, 'class' => array('op')),
      );
    }

  }

  return theme('table', $table);
}

// create symlink in library/haplotype-map, so it can be read
function haplotypemap_create_symlink_when_add_file($source, $destination){
  dpm($source, '$source');
  dpm($destination, '$destination');
  $destination_symlink = $destination.'/'.basename($source);
  file_prepare_directory($destination, FILE_CREATE_DIRECTORY);
  if (file_exists($destination_symlink)){
    return TRUE;
  }else{
    return symlink($source, $destination_symlink);
  }
}
