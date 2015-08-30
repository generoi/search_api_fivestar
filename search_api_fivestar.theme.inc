<?php

/**
 * Returns HTML for an inactive fivestar facet item.
 * @see theme_facetapi_link_inactive().
 */
function theme_search_api_fivestar_facetapi_link_inactive($variables) {
  // Builds accessible markup.
  // @see http://drupal.org/node/1316580
  $accessible_vars = array(
    'text' => $variables['text'],
    'active' => FALSE,
  );
  $accessible_markup = theme('facetapi_accessible_markup', $accessible_vars);

  $variables['text'] = theme('fivestar_static', array(
    'rating' => $variables['rating'] * 20,
    'stars' => $variables['stars'], // @todo should be set on the facet instead as a variable.
    'tag' => 'vote', // @todo should be exposed not only here but for indexing as well.
    'widget' => $variables['widget'],
  ));
  $variables['text'] = check_plain($variables['prefix']) . $variables['text'] . check_plain($variables['suffix']);

  // Adds count to link if one was passed.
  if (isset($variables['count'])) {
    $variables['text'] .= ' ' . theme('facetapi_count', $variables);
  }

  // Resets link text, sets to options to HTML since we already sanitized the
  // link text and are providing additional markup for accessibility.
  $variables['text'] .= $accessible_markup;
  $variables['options']['html'] = TRUE;

  return theme_link($variables);
}

/**
 * Returns HTML for an active fivestar facet item.
 * @see theme_facetapi_link_active().
 */
function theme_search_api_fivestar_facetapi_link_active($variables) {
  // Theme function variables fro accessible markup.
  // @see http://drupal.org/node/1316580
  $accessible_vars = array(
    'text' => $variables['text'],
    'active' => TRUE,
  );
  $link_text = theme('fivestar_static', array(
    'rating' => $variables['rating'] * 20,
    'stars' => $variables['stars'], // @todo should be set on the facet instead as a variable.
    'tag' => 'vote', // @todo should be exposed not only here but for indexing as well.
    'widget' => $variables['widget'],
  ));
  $link_text = check_plain($variables['prefix']) . $link_text . check_plain($variables['suffix']);

  // Builds link, passes through t() which gives us the ability to change the
  // position of the widget on a per-language basis.
  $replacements = array(
    '!facetapi_deactivate_widget' => theme('facetapi_deactivate_widget', $variables),
    '!facetapi_accessible_markup' => theme('facetapi_accessible_markup', $accessible_vars),
  );
  $variables['text'] = t('!facetapi_deactivate_widget !facetapi_accessible_markup', $replacements);
  $variables['options']['html'] = TRUE;

  return theme_link($variables) . $link_text;
}
