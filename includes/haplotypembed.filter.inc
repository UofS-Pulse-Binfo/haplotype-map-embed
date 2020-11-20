<?php




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
      fwrite($handle_out,"$values_4_filtered_germplasm['string_value_in_order']\n");
    }
    fclose($handle_out);
  }
  // save file in temp, create soft link to $out_path, also make it unique
}
