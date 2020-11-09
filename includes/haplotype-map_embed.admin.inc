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

     return $output;
  }

  function haplotypemap_embed_admin_file_form($form, &$form_state) {

    $obj = NULL;

    $form['name'] = array(
      '#type' => 'textfield',
      '#title' => 'Human-readable Name',
      '#description' => 'This is the name shown to your users so make sure it is descriptive and uniquely identifies the VCF file',
      '#default_value' => (isset($obj->name)) ? $obj->name : '',
    );

    $from['file'] = array(
      '#type' => 'textfield',
      '#title' => 'Haplotype map file (absolute path)',
      '#description' => 'The absolute path to your haplotype map file. This file must already exist.',
      '#default_value' => ,
    );

    $from['default_file'] = array(
      '#type' => 'textfield',
      '#title' => 'Default haplotype map file (filtered, only default germplasm, absolute path)',
      '#description' => 'The absolute path to your filtred default hapletype map file. This file must already exist.',
      '#default_value' => ,
    );

    $form['description'] = array(
      '#type' => 'textarea',
      '#title' => 'Description',
      '#description' => 'This should include the information of the file for this haplotype map, also how the file is filtered for deafult file.',
      '#default_value' => (isset($obj->description)) ? $obj->description : '',
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

  }
