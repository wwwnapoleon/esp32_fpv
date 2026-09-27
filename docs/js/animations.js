// ============================================================
//  Анимации появления блоков при скролле
//  Работает везде: hero, карточки, формы, списки, кнопки,
//  заголовки, параграфы — на всех страницах
// ============================================================

(function() {
  'use strict';

  if (!('IntersectionObserver' in window)) {
    // Старый браузер — просто показываем всё
    document.querySelectorAll('*').forEach(el => {
      el.style.opacity = '';
      el.style.transform = '';
    });
    return;
  }

  const OBSERVER_OPTIONS = {
    root: null,
    rootMargin: '0px 0px -30px 0px',
    threshold: 0.05
  };

  // ============================================================
  //  Все селекторы, которые нужно анимировать
  //  Формат: { selector, type, cascade }
  //  type — какой класс эффекта добавить
  //  cascade — включать ли каскадную задержку
  // ============================================================

  const ANIMATION_TARGETS = [
    // Hero
    { selector: '.hero',              type: 'default', cascade: false },

    // Секции целиком
    { selector: '.news-section',      type: 'default', cascade: false },
    { selector: '.team-section',      type: 'default', cascade: false },
    { selector: '.progress-section',  type: 'default', cascade: false },
    { selector: '.about-section',     type: 'default', cascade: false },
    { selector: '.review-section',    type: 'default', cascade: false },
    { selector: '.cta-section',       type: 'default', cascade: false },

    // Заголовки
    { selector: '.page h1',           type: 'default', cascade: false },
    { selector: '.page > .lead',      type: 'default', cascade: false },
    { selector: '.page h2',           type: 'default', cascade: false },
    { selector: '.page h3',           type: 'default', cascade: false },
    { selector: '.page h4',           type: 'default', cascade: false },

    // Параграфы (только вне карточек)
    { selector: '.about-section p',   type: 'default', cascade: true },

    // Карточки — каскад
    { selector: '.news-card',         type: 'default', cascade: true },
    { selector: '.team-card',         type: 'default', cascade: true },
    { selector: '.progress-list li',  type: 'default', cascade: true },

    // Раскрывающиеся карточки — анимируем summary, не details!
    { selector: 'details.direction-card > summary',
      type: 'default', cascade: true },
    { selector: 'details.action-card > summary',
      type: 'default', cascade: true },
    { selector: 'details.blog-post > summary',
      type: 'default', cascade: true },

    // Блоки внутри раскрытых карточек
    { selector: '.direction-content > *',
      type: 'default', cascade: true },
    { selector: '.action-content > *',
      type: 'default', cascade: true },
    { selector: '.post-content > *',
      type: 'default', cascade: true },

    // Статистика и теги
    { selector: '.action-stats',      type: 'default', cascade: false },
    { selector: '.post-meta',         type: 'default', cascade: false },
    { selector: '.post-tags',         type: 'default', cascade: false },

    // Формы
    { selector: '.review-form',       type: 'default', cascade: false },
    { selector: '.input-group',       type: 'default', cascade: true },

    // Кнопки
    { selector: '.btn-primary',       type: 'default', cascade: false },
    { selector: '.hero-actions .btn', type: 'default', cascade: true }
  ];

  // ============================================================
  //  Один глобальный observer
  // ============================================================

  const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
      if (entry.isIntersecting) {
        entry.target.classList.add('visible');
        observer.unobserve(entry.target);
      }
    });
  }, OBSERVER_OPTIONS);

  // ============================================================
  //  Пометить и наблюдать
  // ============================================================

  function markAndObserve() {
    ANIMATION_TARGETS.forEach(target => {
      let elements;
      try {
        elements = document.querySelectorAll(target.selector);
      } catch (e) {
        console.warn('Неверный селектор:', target.selector);
        return;
      }

      elements.forEach((el, index) => {
        // Пропускаем, если уже помечен
        if (el.classList.contains('animate-on-scroll')) return;

        el.classList.add('animate-on-scroll');

        // Каскадная задержка (1-6)
        if (target.cascade) {
          const delay = (index % 6) + 1;
          el.classList.add('delay-' + delay);
        }

        // Наблюдать
        observer.observe(el);

        // Сразу видимый, если уже в зоне видимости
        const rect = el.getBoundingClientRect();
        const inView = rect.top < window.innerHeight && rect.bottom > 0;
        if (inView) {
          requestAnimationFrame(() => {
            requestAnimationFrame(() => {
              el.classList.add('visible');
            });
          });
        }
      });
    });
  }

  // ============================================================
  //  Экспорт
  // ============================================================

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
