// ============================================================
//  Анимации появления блоков при скролле
//  Используется IntersectionObserver — без библиотек
//  Работает и при первой загрузке, и после динамической
//  подгрузки контента (pages.js)
// ============================================================

(function() {
  'use strict';

  // Проверка поддержки
  if (!('IntersectionObserver' in window)) {
    document.querySelectorAll('.animate-on-scroll, .hero').forEach(el => {
      el.classList.add('visible');
    });
    return;
  }

  const OBSERVER_OPTIONS = {
    root: null,
    rootMargin: '0px 0px -30px 0px',
    threshold: 0.05
  };

  // Что анимировать
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
  ].join(', ');

  // Один глобальный observer — наблюдаем все элементы
  const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
      if (entry.isIntersecting) {
        entry.target.classList.add('visible');
        observer.unobserve(entry.target);
      }
    });
  }, OBSERVER_OPTIONS);

  // Пометить элементы + сразу проверить видимость
  function markAndObserve() {
    document.querySelectorAll(SELECTORS).forEach((el, index) => {
      if (el.classList.contains('animate-on-scroll')) return;

      el.classList.add('animate-on-scroll');

      // Каскадная задержка
      const delay = (index % 6) + 1;
      el.classList.add('delay-' + delay);

      // Наблюдать
      observer.observe(el);

      // Если элемент уже в зоне видимости — сразу видимый
      const rect = el.getBoundingClientRect();
      const inView = rect.top < window.innerHeight && rect.bottom > 0;
      if (inView) {
        // Задержка, чтобы дать браузеру отрисовать начальное состояние
        requestAnimationFrame(() => {
          requestAnimationFrame(() => {
            el.classList.add('visible');
          });
        });
      }
    });
  }

  // Экспорт
  window.initScrollAnimations = function() {
    markAndObserve();
  };

  // Первичный запуск
  document.addEventListener('DOMContentLoaded', () => {
    setTimeout(() => {
      window.initScrollAnimations();
    }, 50);
  });

})();
