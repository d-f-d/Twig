<?php
namespace dfd\Twig;

use Drupal\Core\Render\RenderableInterface;
use Twig_Extension;
use Twig_SimpleFilter;
use Twig_SimpleFunction;

class Extension extends Twig_Extension {

  /**
   * @return array
   */
  public function getFilters() {
    return [
      new Twig_SimpleFilter('check_markup', 'check_markup', ['is_safe' => ['html']]),
      new Twig_SimpleFilter('text_summary', 'text_summary', ['is_safe' => ['html']]),
    ];
  }

  /**
   * @return array
   */
  public function getFunctions() {
    return [
      new Twig_SimpleFunction('render_fragment', [$this, 'renderFragment'], ['is_safe' => ['html']]),
      new Twig_SimpleFunction('render_fragment_deferred', [$this, 'renderFragmentDeferred'], ['is_safe' => ['html']]),
    ];
  }

  /**
   * @param string $class
   * @param array $context
   * @return RenderableInterface|string
   */
  public function renderFragment($class, $context = []) {
    if ((new \ReflectionClass($class))->implementsInterface('Drupal\\Core\\Render\\RenderableInterface')) {
      $args = func_get_args();
      array_shift($args);
      return (new \ReflectionClass($class))->newInstanceArgs($args);
    }
    return '';
  }

  /**
   * @param string $class
   * @return array|string
   */
  public function renderFragmentDeferred($class) {
    if ((new \ReflectionClass($class))->implementsInterface('Drupal\\Core\\Render\\RenderableInterface')) {
      $args = func_get_args();
      array_shift($args);
      return [
        '#lazy_builder'       => [$class . '::deferred', $args],
        '#create_placeholder' => TRUE,
      ];
    }
    return '';
  }

  /**
   * @return string
   */
  public function getName() {
    return __CLASS__;
  }
}
