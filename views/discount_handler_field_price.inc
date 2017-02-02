<?php

/**
 * @file
 * Views handler: Product price field.
 */

/**
 * Returns a formatted price value to display in the View.
 */
class discount_handler_field_price extends views_handler_field_numeric {

  /**
   * Overrides views_handler::option_definition().
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['format'] = array('default' => 'bc_price');

    return $options;
  }

  /**
   * Overrides views_handler::options_form().
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $options = $this->options;

    $form['format'] =  array(
      '#title' => t('Format'),
      '#type' => 'radios',
      '#options' => array(
        'bc_price' => t('discount price'),
        'numeric' => t('Numeric'),
      ),
      '#default_value' => $options['format'],
      '#weight' => 1,
    );

    // Change weight and dependency of the previous field on the parent numeric ones
    $weight = 2;
    foreach (array('set_precision', 'precision', 'decimal', 'separator', 'prefix', 'suffix') as $field) {
      if (isset($form[$field]['#dependency'])) {
        $form[$field]['#dependency'] += array('radio:options[format]' => array('numeric'));
        $form[$field]['#dependency_count'] = count($form[$field]['#dependency']);
      }
      else {
        $form[$field]['#dependency'] = array('radio:options[format]' => array('numeric'));
      }

      $form[$field]['#weight'] = ++$weight;
    }
  }

  /**
   * Overrides views_handler_field::render().
   */
  function render($values) {
    if (discount_is_product($values->{$this->aliases['type']})) {
      if ($this->options['format'] == 'numeric') {
        return parent::render($values);
      }
  
      if ($this->options['format'] == 'bc_price') {
        $value = $this->get_value($values);
  
        if (is_null($value) || ($value == 0 && $this->options['empty_zero'])) {
          return '';
        }
        // Set the price format.
        $value = discount_price_format($value);
        return theme('discount_price', array('price' => $value));
      }
    }
  }
}