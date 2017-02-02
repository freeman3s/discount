<?php

/**
 * @file
 * Views hooks for discount products.
 */

/**
 * Implements hook_views_data_alter().
 */
function discount_views_data_alter(&$data) {
  // alter a price field 
  if (isset($data['field_data_price'])){
    $data['field_data_price']['price']['group'] = t('Product');
    $data['field_data_price']['price']['field']['handler'] = 'discount_handler_field_price';
    $data['field_data_price']['price']['field']['float'] = TRUE;
    $data['field_data_price']['price']['field']['additional fields']['nid'] = array(
                                                                                    'table' => 'node',
                                                                                    'field' => 'nid'
                                                                                    );
    $data['field_data_price']['price']['field']['additional fields']['type'] = array(
                                                                                    'table' => 'node',
                                                                                    'field' => 'type'
                                                                                    );
  }
  // we haven't own custom table, so alter node table
  $data['node']['is_product'] = array(
    'title' => t('Is a product'),
    'group' => t('Product'),
    'help' => t('Check for the fields provided by the discount module.'),
    'filter' => array(
      'handler' => 'discount_handler_filter_product',
      'label' => t('Is a product'),
    ),
  );
}
