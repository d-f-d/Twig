<?php

namespace d\Twig;

use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Template\TwigEnvironment;
use Twig_LoaderInterface;


class Twig extends TwigEnvironment {

  function __construct($root, CacheBackendInterface $cache, $twig_extension_hash, Twig_LoaderInterface $loader, array $options) {
    parent::__construct($root, $cache, $twig_extension_hash, $loader, $options);
    $this->addExtension(new Extension());
    $this->addTokenParser(new BEM\TokenParser('b'));
    $this->addTokenParser(new BEM\TokenParser('e', 'element'));
  }
}