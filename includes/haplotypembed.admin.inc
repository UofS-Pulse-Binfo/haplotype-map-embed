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

     $output .= '<p>This module provides a form for users to utilize haplotype datasite visulation module haplotype map.:</p>';

     $output .= l('Add', 'admin/tripal/extension/haplotypemap_embed/add');

     $table = array(
       'header' => array('Human-readable Name', 'Number of Germplasm', 'Germplasm in Default File'),
       'rows' => array(),
     );

     $results = db_query('SELECT * FROM {haplotypembed_files}');
     foreach ($results as $r) {
       // count number of germplasm first

       $table['rows'][] = array(
         'name' => $r->human_readable_name,
         'total_number_germplasm' => '100',
         'file' => $r->file_path,
         l('edit', 'admin/tripal/extension/haplotypemap_embed/edit/'.$r->haplotypembed_file_id) .' | '
           . l('delete', 'admin/tripal/extension/haplotypemap_embed/delete/'.$r->haplotypembed_file_id) .' | '
           . l('access', 'admin/tripal/extension/haplotypemap_embed/access/'.$r->haplotypembed_file_id)
       );
     }
     // If there are no VCF files yet, then tell the admin this.
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
        '#suffix' => '<span> ' . l('or Cancel', '/admin/tripal/extension/haplotypemap_embed') . '</span>',
      );

      return $form;
    }

    $form['name'] = array(
      '#type' => 'textfield',
      '#title' => 'Human-readable Name',
      '#description' => 'This is the name shown to your users so make sure it is descriptive and uniquely identifies the VCF file',
      '#default_value' => (isset($obj->human_readable_name)) ? $obj->human_readable_name : '',
    );

    $from['file'] = array(
      '#type' => 'textfield',
      '#title' => 'Haplotype map file (absolute path)',
      '#description' => 'The absolute path to your haplotype map file. This file must already exist.',
      '#default_value' => (isset($obj->file_path)) ? $obj->file_path : '',
    );

    $from['default_file'] = array(
      '#type' => 'textfield',
      '#title' => 'Default haplotype map file (filtered, only default germplasm, absolute path)',
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
    $form_state['redirect'] = 'admin/tripal/extension/haplotypemap_embed';

    if ($form_state['values']['op'] == 'Delete') {
      db_query('DELETE FROM {haplotypembed_files} WHERE haplotypembed_file_id=:id',
        array(':id' => $form_state['values']['haplotypembed_file_id']));
      // db_query('DELETE FROM {haplotypembed_perm} WHERE haplotypembed_file_id=:id',
      //   array(':id' => $form_state['values']['haplotypembed_file_id']));
      drupal_set_message('Successfully deleted <em>'.$form_state['values']['name'].'</em>');
    }
    else {
      $record = array(
        'file_path' => $form_state['values']['file'],
        'default_file_path' => $form_state['values']['default_file'],
        'description' => str_replace(',','', $form_state['values']['description']),
        'human_readable_name' => $form_state['values']['assembly'],
        'all_germplasm' => $form_state['values']['name'],
        'default_germplasm' => $form_state['values']['default_germplasm'],
      );

      if ($form_state['values']['op'] == 'Add') {
        drupal_write_record('haplotypembed_files', $record);
      }
      else {
        $record['vcf_file_id'] = $form_state['values']['haplotypembed_file_id'];
        drupal_write_record('haplotypembed_files', $record, 'haplotypembed_file_id');
      }
    }
  }
