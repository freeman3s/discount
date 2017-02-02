<?php
/**
 * @file
 * discount main theme function wrapper.
 */

module_load_include('inc', 'discount', 'discount.cart');

/**
 * Theme function implementation.
 */
function discount_price($vars) {
  $price = $vars['price'];
  $html = '<span class="discount-price">' . $price . '</span>';
  return $html;
}

/**
 * Theme function implementation.
 */
function discount_add_to_cart_button($vars) {
  $path = $vars['path'];
  $css_class = $vars['css_class'];
  $options = array(
    'attributes' => array(
      'class' => array($css_class)
    )
  );
  return '<div class="discount-add-to-cart">' . l(t('Add to cart'), $path, $options) . '</div>';
}

/**
 * Theme implementation for rendering the total price.
 * 
 * @todo find a way to pass $price through the variables array.
 */
function theme_discount_cart_total_price($variables) {
  $price = discount_get_total_price();
  $total = discount_price_format($price->total);
  
  // Building the HTML.
  $html  = '<div class="discount-cart-total-price-contents row">';
  $html .= '  <div class="discount-total-price cell">' . t('Total price') . ': <strong>' . $total . '</strong></div>';
  $html .= '</div>';
  
  $vat_is_enabled = (int) variable_get('discount_vat_state');
  if (!empty ($vat_is_enabled) && $vat_is_enabled) {
    $vat_value = discount_price_format($price->vat);
    $html .= '<div class="discount-cart-total-vat-contents row">';
    $html .= '  <div class="discount-total-vat cell">' . t('Total VAT') . ': <strong>' . $vat_value . '</strong></div>';
    $html .= '</div>';
  }
  
  return $html;
}

/**
 * Theme implementation for rendering the cart element.
 */
function theme_discount_render_cart_element($variables) {
  // Element name and nid.
  $name = $variables['form']['#name'];
  $nid = (int) str_replace(array('cartcontents[', ']'), '', $name);
  if (empty($nid)) {
    return '';
  }
  // Delete image.
  $vars = array(
    'path' => base_path() . drupal_get_path('module', 'discount') . '/images/delete2.png',
    'alt' => t('Remove from cart'),
    'title' => t('Remove from cart'),
    'attributes' => array('class' => 'discount-delete-image-image'),
  );
  $delete_link = l(theme('image', $vars), 'cart/remove/' . $nid, array('html' => TRUE));
  // Getting the node for it's title and description.
  $node = discount_get_cart($nid);
  // Node description.
  $desc = '';
  if (isset($node->discount_node_description)) {
    $desc = drupal_strlen($node->discount_node_description) > 50 ?
                truncate_utf8($node->discount_node_description, 50) : $node->discount_node_description;
  }
  // Price and currency.
  $unit_price = isset($node->discount_unit_price) ? $node->discount_unit_price : 0;
  $unit_price = discount_price_format($unit_price);
  
  // Prefix.
  $prefix  = '<div class="discount-cart-contents row">';
  $prefix .= '  <div class="discount-delete-image cell">' . $delete_link . '</div>';
  $prefix .= '  <div class="discount-cart-node-title cell">' . l($node->title, 'node/' . $nid) . '<br />';
  $prefix .= '    <span class="discount-cart-node-summary">' . $desc . '</span>';
  $prefix .= '  </div>';
  $prefix .= '  <div class="cell discount-cart-unit-price"><strong>' . $unit_price . '</strong></div>';
  $prefix .= '  <div class="cell discount-cart-x">x</div>';
  $prefix .= '  <div class="discount-cart-quantity cell">';
  $prefix .= '    <div class="cell">';
  // Suffix.
  $suffix  = '    </div>';
  $suffix .= '  </div>';
  $suffix .= '</div>';
  
  // Rendering the element as textfield.
  $quantity = theme('textfield', $variables['form']);
  // Full view return.
  return $prefix . $quantity . $suffix;
}
