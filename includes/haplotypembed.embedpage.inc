<?php

/**
 * Provides functionality for the embedded page.
 */

/**
 * Embeds Haplotype map in the current page.
 */
function haplotypembed_embed_page() {
  $output = '';

  $haplotypemap_embed_lib_path = libraries_get_path('haplotype-map') . '/';
  drupal_add_js($haplotypemap_embed_lib_path . 'dist/bundle/main.bundle.1cd727076111f0e998e1.js');
  $output .= '<div id="root"></div>';

  return $output;
}
