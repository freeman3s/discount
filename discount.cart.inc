<?php

/**
 * @file
 * discount shopping cart implementation functions.
 */

/**
 * Callback function for cart listing.
 */
function discount_cart() {
  $cart = discount_get_cart();

  if (empty($cart)) {
    return t('Your cart is empty.');
  }
  return drupal_get_form('discount_cart_form');
}

/**
 * Shopping cart form.
 */
function discount_cart_form() {
  // Getting the shopping cart.
  $cart = discount_get_cart();
  // And now the form.
  $form['cartcontents'] = array(
    // Make the returned array come back in tree form.
    '#tree' => TRUE,
    '#prefix' => '<div class="discount-cart discount-grid">',
    '#suffix' => '</div>',
  );
  // Cart elements.
  foreach ($cart as $nid => $node) {
    $form['cartcontents'][$nid] = array(
      '#type' => 'textfield',
      '#size' => 1,
      '#default_value' => $node->discount_quantity,
      '#theme' => 'discount_render_cart_element',
    );
  }
  // Total price.
  $form['total_price'] = array(
    '#markup' => t('Total price'),
    '#prefix' => '<div class="discount-cart discount-grid">',
    '#suffix' => '</div>',
    '#theme' => 'discount_cart_total_price',
  );
  // Buttons.
  $form['buttons'] = array(
    // Make the returned array come back in tree form.
    '#tree' => TRUE,
    '#prefix' => '<div class="row"><div class="discount-call-to-action cell">',
    '#suffix' => '</div></div>',
  );
  // Update button.
  $form['buttons']['update'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  
  // Checkout button.
  if (module_exists('discount_order')) {
    $form['buttons']['checkout'] = array(
      '#type' => 'submit',
      '#value' => t('Checkout'),
    );
  }

  return $form;
}

/**
 * Shopping cart form.
 */
function discount_cart_form_submit($form_id, $form_state) {
  foreach ($form_state['values']['cartcontents'] as $nid => $value) {
    $quantity = (int) $value;
    if ($quantity > 0) {
      $_SESSION['discount']['cart'][$nid]->discount_quantity = $quantity;
    }
    // If the quantity is zero, we just remove the node from the cart.
    elseif ($quantity == 0) {
      unset($_SESSION['discount']['cart'][$nid]);
    }
  }
  
  if (module_exists('discount_order') && $form_state['values']['op'] == t('Checkout')) {
    drupal_goto('checkout');
  }
  else {
    drupal_set_message(t('Shopping cart updated.'));
  }
}

/**
 * Callback function for cart/add/.
 *
 * @param int $nid
 *   We are using the node id to store the node in the shopping cart
 */
function discount_add_to_cart($nid = NULL, $quantity = 1) {
  $nid = (int) $nid;
  $quantity = (int) $quantity;
  if ($nid > 0 && $quantity > 0) {
    // If a node is added more times, just update the quantity.
    $cart = discount_get_cart();

    $node = node_load($nid);
    global $user;
    $query = db_select('nodeviewcount', 'nvc');
    $query->condition('nid', $node->nid, '=');
    $query->condition('uid', $user->uid, '=');
    $query->addExpression('COUNT(*)');
    $result = (int)$query->execute()->fetchField();

    if (!empty($cart) && in_array($nid, array_keys($cart))) {
      if ($result == 3) {
        // Unit price.
        $unit_price = field_get_items('node', $node, 'price');
        $unit_price = isset($unit_price[0]['value']) ? check_plain(strip_tags($unit_price[0]['value'])) : 0;
        $node->discount_unit_price = $unit_price * 0.9;
        $_SESSION['discount']['cart'][$nid] = $node;
      }
      // Clicked 2 times on add to cart button. Increment quantity.
      $_SESSION['discount']['cart'][$nid]->discount_quantity += $quantity;
    }
    else{
      $node = node_load($nid);
      $node->discount_quantity = $quantity;
      // Adding description.
      $body = field_get_items('node', $node, 'body');
      $description = isset($body[0]['value']) ? check_plain(strip_tags($body[0]['value'])) : '';
      $node->discount_node_description = $description;
      // Unit price.
      $unit_price = field_get_items('node', $node, 'price');
      $unit_price = isset($unit_price[0]['value']) ? check_plain(strip_tags($unit_price[0]['value'])) : 0;

      if ($result >= 3) {
        $node->discount_unit_price = $unit_price * 0.9;
      } else {
        $node->discount_unit_price = $unit_price;
      }

      // Storing in session.
      $_SESSION['discount']['cart'][$nid] = $node;

    }
  }
  drupal_set_message(t('Shopping cart updated.'));

  $redirect = variable_get('discount_redirect_user_after_add_to_cart', FALSE);
  if (empty($redirect)) {
    drupal_goto('cart');
  }
  elseif ($redirect == '<none>') {
    $referer = $_SERVER["HTTP_REFERER"];
    drupal_goto($referer);
  }
  else {
    drupal_goto($redirect);
  }
}

/**
 * Callback function for cart/remove/.
 *
 * @param int $nid
 *   We are using the node id to remove the node in the shopping cart
 */
function discount_remove_from_cart($nid = NULL) {
  $nid = (int) $nid;
  if ($nid > 0) {
    unset($_SESSION['discount']['cart'][$nid]);
  }
  drupal_set_message(t('Shopping cart updated.'));
  drupal_goto('cart');
}

/**
 * Function for shopping cart retrieval.
 *
 * @param int $nid
 *   We are using the node id to store the node in the shopping cart
 *
 * @return mixed
 *   Returning the shopping cart contents.
 *   An empty array if there is nothing in the cart
 */
function discount_get_cart($nid = NULL) {
  if (isset($nid)) {
    return $_SESSION['discount']['cart'][$nid];
  }
  if (isset($_SESSION['discount']['cart'])) {
    return $_SESSION['discount']['cart'];
  }
  // Empty cart.
  return array();
}

/**
 * Returns the final price for the shopping cart.
 *
 * @return mixed $total_price
 *   The total price for the shopping cart. 
 */
function discount_get_total_price() {
  // Building the return array.
  $return = array(
    'price' => 0,
    'vat' => 0,
    'total' => 0,
  );
  $cart = discount_get_cart();
  if (empty($cart)) {
    return (object) $return;
  }
  
  $total_price = 0;
  foreach ($cart as $nid => $node) {
    if (isset($node->discount_quantity) && isset($node->discount_unit_price)) {
      $total_price += $node->discount_quantity * $node->discount_unit_price;
    }
  }
  
  $return['price'] = $total_price;
  
  // Checking whether to apply the VAT or not.
  $vat_is_enabled = (int) variable_get('discount_vat_state');
  if (!empty ($vat_is_enabled) && $vat_is_enabled) {
    $vat_value = (float) variable_get('discount_vat_value');
    $vat_value = ($total_price * $vat_value) / 100;
    $total_price += $vat_value;
    // Adding VAT and total price to the return array.
    $return['vat'] = $vat_value;
  }
  
  $return['total'] = $total_price;
  return (object) $return;
}

/**
 * Shopping cart reset.
 */
function discount_empty_cart() {
  unset($_SESSION['discount']['cart']);
}

/**
 * Formats the input $price in the desired format.
 *
 * @param float $price
 *   The price in the raw format.
 * @return $price
 *   The price in the custom format.
 */
function discount_price_format($price) {
  $format = variable_get('discount_price_format');
  $currency = check_plain(variable_get('discount_currency'));
  $price = (float) $price;
  switch ($format) {
    case 0:
      $price = number_format($price, 2, ',', ' ') . ' ' . $currency;
      break;
    
    case 1:
      $price = number_format($price, 2, '.', ' ') . ' ' . $currency;
      break;
    
    case 2:
      $price = number_format($price, 2, '.', ',') . ' ' . $currency;
      break;
    
    case 3:
      $price = number_format($price, 2, ',', '.') . ' ' . $currency;
      break;
    
    case 4:
      $price = $currency . ' ' . number_format($price, 2, ',', ' ');
      break;
    
    case 5:
      $price = $currency . ' ' . number_format($price, 2, '.', ' ');
      break;
    
    case 6:
      $price = $currency . ' ' . number_format($price, 2, '.', ',');
      break;
    
    case 7:
      $price = $currency . ' ' . number_format($price, 2, ',', '.');
      break;
    
    default:
      $price = number_format($price, 2, ',', ' ') . ' ' . $currency;
      break;
  }
  return $price;
}

/**
 * Returns the available price formats.
 *
 * @return $formats
 *   A list with the available price formats.
 */
function _discount_price_format() {
  $currency = variable_get('discount_currency');
  return array(
    0 => t('1 234,00 @currency', array('@currency' => $currency)),
    1 => t('1 234.00 @currency', array('@currency' => $currency)),
    2 => t('1,234.00 @currency', array('@currency' => $currency)),
    3 => t('1.234,00 @currency', array('@currency' => $currency)),
    
    4 => t('@currency 1 234,00', array('@currency' => $currency)),
    5 => t('@currency 1 234.00', array('@currency' => $currency)),
    6 => t('@currency 1,234.00', array('@currency' => $currency)),
    7 => t('@currency 1.234,00', array('@currency' => $currency)),
  );
}