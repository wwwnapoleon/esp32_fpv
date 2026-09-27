// ============================================================
//  Анимации появления блоков при скролле
//  Используется IntersectionObserver — без библиотек
// ============================================================

(function() {
  'use strict';

  // Проверка поддержки
  if (!('IntersectionObserver' in window)) {
    // Старый браузер — просто показываем всё сразу
    document.querySelectorAll('.animate-on-scroll, .hero').forEach(el => {
      el.classList.add('visible');
    });
    return;
  }

  // ---------- Настройки ----------
  const OBSERVER_OPTIONS = {
    root: null,               // наблюдать относительно окна
    rootMargin: '0px 0px -50px 0px',  // сработает чуть раньше, чем элемент полностью в кадре
    threshold: 0.1            // достаточно 10% видимости
  };

  // ---------- Что анимировать ----------
  // Все элементы, которым нужно плавное появление
  const SELECTORS = [
    '.hero',
    '.news-card',
    '.team-card',
    '.progress-list li',
    '.direction-card',
    '.action-card',
    '.blog-post',
    '.review-section',
    '.cta-section',
    '.about-section',
    '.page h1',
    '.page > .lead',
    '.page h2'
  ];

  // ---------- Пометить элементы ----------
  function markElements() {
    const selector = SELECTORS.join(', ');
    document.querySelectorAll(selector).forEach((el, index) => {
      // Пропустить, если уже помечен
      if (el.classList.contains('animate-on-scroll')) return;
      if (el.tagName === 'DETAILS') {
        // details нельзя трансформировать — иначе ломается раскрытие
        // анимируем только при появлении, добавляя visible
      }

      el.classList.add('animate-on-scroll');

      // Каскадная задержка (1-6)
      const delay = (index % 6) + 1;
      el.classList.add('delay-' + delay);
    });
  }

  // ---------- Наблюдатель ----------
  const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
      if (entry.isIntersecting) {
        entry.target.classList.add('visible');
        observer.unobserve(entry.target);   // анимируем один раз
      }
    });
  }, OBSERVER_OPTIONS);

  // ---------- Запустить ----------
  function observeAll() {
    document.querySelectorAll('.animate-on-scroll').forEach(el => {
      observer.observe(el);
    });
  }

  // ---------- Экспорт для использования из pages.js ----------
  window.initScrollAnimations = function() {
    markElements();
    observeAll();
  };

  // ---------- Первичная инициализация ----------
  document.addEventListener('DOMContentLoaded', () => {
    // Небольшая задержка, чтобы контент успел загрузиться
    setTimeout(() => {
      window.initScrollAnimations();
    }, 100);
  });

})();
