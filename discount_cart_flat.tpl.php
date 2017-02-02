<?php
/**
 * @file
 * discount shopping cart html template
 */
?>

<?php if (empty($cart)): ?>
  <p><?php print t('Your shopping cart is empty.'); ?></p>
<?php else: ?>
  <div class="discount-cart discount-grid">
    <?php if(is_array($cart) && count($cart) >= 1): ?>
      <?php foreach($cart as $nid => $node): ?>
        <div class="discount-cart-contents row">
            
          <div class="cell discount-cart-unit-price">
            <?php if (!isset($node->discount_unit_price)) $node->discount_unit_price = 0; ?>
            <strong><?php print discount_price_format($node->discount_unit_price); ?></strong>
          </div>
          <div class="cell discount-cart-x">x</div>
          <div class="discount-cart-quantity cell"><?php print $node->discount_quantity; ?></div>
          
          <div class="discount-cart-node-title cell">
            <?php print l($node->title, 'node/' . $node->nid); ?>
          </div>
        </div>
      <?php endforeach; ?>
   </div> 
   <div class="discount-cart discount-grid">
      <div class="discount-cart-total-price-contents row">
        <div class="discount-total-price cell">
          <?php print t('Total price'); ?>: <strong> <?php print $price; ?></strong>
        </div>
      </div>
    
      <?php if (!empty ($vat)): ?>
        <div class="discount-cart-total-vat-contents row">
          <div class="discount-total-vat cell"><?php print t('Total VAT'); ?>: <strong><?php print $vat; ?></strong></div>
        </div>
      <?php endif; ?>
    <?php endif; ?>
  </div>
<?php endif; ?>
