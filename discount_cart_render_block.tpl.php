<?php
/**
 * @file
 * discount shopping cart block
 */
?>

<?php if (empty($cart)): ?>
  <p><?php print t('Your cart is empty.'); ?></p>
<?php else: ?>
  <div class="discount-grid discount-block">
    <?php if(is_array($cart) && count($cart) >= 1): ?>
      <?php foreach($cart as $nid => $node): ?>
        <div class="discount-cart-contents row">
          <div class="discount-cart-node-title cell"><?php print l($node->title, 'node/' . $node->nid); ?></div>
        
          <div class="discount-cart-quantity cell"><?php print $node->discount_quantity; ?></div>
          <div class="discount-cart-x cell">x</div>
          <div class="discount-cart-unit-price cell">
            <strong><?php print discount_price_format($node->discount_unit_price); ?></strong>
          </div>
            
        </div>
      <?php endforeach; ?>
      <div class="discount-cart-total-price-contents row">
        <div class="discount-total-price cell">
          <?php print t('Total price'); ?>:<strong> <?php print $price; ?></strong>
        </div>
      </div>
      <?php if (!empty ($vat)): ?>
        <div class="discount-block-total-vat-contents row">
          <div class="discount-total-vat cell"><?php print t('Total VAT'); ?>: <strong><?php print $vat; ?></strong></div>
        </div>
      <?php endif; ?>
      <div class="discount-cart-checkout-button discount-cart-checkout-button-block row">
        <?php print l(t('View cart'), 'cart', array('attributes' => array('class' => array('button')))); ?>
      </div>
    <?php endif; ?>
  </div>
<?php endif; ?>
