<?php

 /**
  * Implements hook_register_trpdownload_type().
  */
 function haplotypembed_register_trpdownload_type(){
   $types = array();
   $types['haplotypembed_filter'] = array(
     'type_name' => 'Filter Hapmap',
     'format' => 'Hapmap Format',
     'functions' => array(
       'generate_file' => 'haplotypembed_filter_generate_file',
       'summarize' => 'haplotypembed_filter_summary',
     ),
   );
   return $types;
 }


/**
 * Generates the file.
 *
 * @param $variables
 *   An associative array of parameters including:
 *     - q: all the query paramters.
 *     - site_safe_name: a sanitized version of your site name for use in variables & filenames.
 *     - type_info: an array of info for the download type.
 *     - suffix: the file format suffix.
 *     - filename: the filename of the file to generate not including path.
 *     - fullpath: the full path where the file should be generated.
 *     - format_name: a human-readable description of the format.
 * @param $job_id
 *   The ID of the tripal job executing this function ;-).
 */
function haplotypembed_filter_generate_file($variables, $job_id = NULL){
  dpm($variables, '$variables in haplotypembed_filter_generate_file ');
  //test
  print_r($variables);
  // Create the file and ready it for writting to.
  $filepath = $variables['fullpath'] . $variables['filename'];
  print "File: ".$filepath."\n";
  // Get the VCF file details.
  $one_hapmap_file = get_one_haplotype_map_file($variables['q']['haplotypembed_file_id']);
  print "\nHapmap File (".$one_hapmap_file->human_readable_name."): ".$one_hapmap_file->file_path."\n";
  // Check the input file still exists.
  if (!file_exists($one_hapmap_file->file_path)) {
    drush_set_error("ERROR: Original Hapmap file not found!");
    exit(2);
  }

  // Print out parameters.
  print "Parameters:\n";
  foreach ($variables['q'] as $label => $value) {
    print '  ' . $label . ': ' . $value."\n";
  }

  // Give the user some indication we've done something...
  tripal_set_job_progress($job_id, 10);

  // Filter.
  //===============
  print "\nFiltering the Hapmap File by Germplasm\n";
  // hapmap_filter_by_germplasm($haplotypembed_file_id, $in_germplasm_wanted, $out_path)
  $filtered_hapmap = hapmap_filter_by_germplasm($variables['q']['haplotypembed_file_id'], $variables['q']['selected_germplasm'], $filepath);
  if (!$filtered_hapmap) {
    exit(3);
  }
  tripal_set_job_progress($job_id, 90);

  // Format. create symlink in libraries/haplotype-map/dist/data/filtered

  //print "\n \n";
  //shell_exec("ln");
  if (!file_exists( $filepath )) {
    drush_set_error("ERROR: Unable to move Filtered VCF into expected file director: $filepath");
    exit(5); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
  }

  // Done!
  tripal_set_job_progress($job_id, 100);
}

/**
 * Summarize the current download for filtered hapmap file.
 *
 * @param $vars
 *   An array of variables available to this function. Including:
 *     - ['download_args']['q']: all the arguments in the path passed to the Tripal Download API
 *     - ['download_args']['safe_site_name']: a sanitized version of the site-name for use in filenames
 *     - ['download_args']['type_info']: what you defined in hook_register_trpdownload_type() for the current download type
 * @return
 *   A string representing the summary (can include html).
 */
function haplotypembed_filter_summary($vars){


}

/**
 * hapmap filter by germplasm
 */
function hapmap_filter_by_germplasm($haplotypembed_file_id, $in_germplasm_wanted, $out_path){

  $file_need_filter = get_one_haplotype_map_file($haplotypembed_file_id);

  $out_hapmap_filename = $out_path . basename($file_need_filter['file_path']);
  //write into path using same name from inpufile
  $handle_out = fopen($out_hapmap_filename,'w') or die ("\nCannot create file: $out_hapmap_filename .\n");

  print 'Haplotype map ' . $file_need_filter->human_readable_name . 'will be filtered' . "\n";

  // trim each germplasm
  $wanted_germplasm = explode("\n", $in_germplasm_wanted);
  for($i = 0; $i < count($wanted_germplasm); $i++){
    $wanted_germplasm[$i] = trim($wanted_germplasm[$i]);
    //$wanted_germplasm_in_order[ $wanted_germplasm[$i] ] = $wanted_germplasm[$i];
  }

  // read the file, print into new file in order of user selected germplasm
  foreach (new SplFileObject($in_vcf_filename) as $line) {
    $line = trim($line);
    if (preg_match('/^rs#/', $line)) {
      // determine the germplasm column number in hapmap file first
      $header = explode("\t", $line);
      for ($j = 11; $j < count($header); $j++){
        if ( in_array($header[$j], $wanted_germplasm) ){
          $flag_germplasm_column_order[$j] = $header[$j];
        }
      }
      //print header into the file
      fwrite($handle_out, ("rs#\talleles\tchrom\tpos\tstrand\tassembly#\tcenter\tprotLSID\tassayLSID\tpanel\tQCcode\t"));
      fwrite($handle_out, (improde("\t", $flag_germplasm_column_order)."\n"));
    }
    else{
      $values_4_filtered_germplasm['string_value_in_order'] = '';
      $one_line = explode("\t", $line);
      for ($i = 0; $i < 11; $i++){
        fwrite($handle_out, "$one_line[$i]\t");
      }
      for ($j = 11; $j < count($one_line); $j++){
        if (array_key_exists($j, $flag_germplasm_column_order)){
          $values_4_filtered_germplasm[ $flag_germplasm_column_order[$j] ] = $one_line[$j];
        }
      }
      for($i=0; $i<(count($wanted_germplasm)-1); $i++){
        $values_4_filtered_germplasm['string_value_in_order'] .= $values_4_filtered_germplasm[ $wanted_germplasm[$i] ];
        $values_4_filtered_germplasm['string_value_in_order'] .= "\t";
      }
      $values_4_filtered_germplasm['string_value_in_order'] .= $values_4_filtered_germplasm[ $wanted_germplasm[count($wanted_germplasm)] ];
      fwrite($handle_out, ($values_4_filtered_germplasm['string_value_in_order']."\n"));
    }
    fclose($handle_out);
  }
  // save file in temp, create soft link to $out_path, also make it unique
}
