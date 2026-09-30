// ============================================================
//  Динамическая загрузка страниц
// ============================================================

const contentArea = document.getElementById('content-area');
const pageTitle   = document.getElementById('page-title');

const PAGE_TITLES = {
  'directions': '🎯 Направления',
  'actions':    '♻️ Акции',
  'zhiza':      '📰 ЖИЗА',
  'news':       '📰 Новости проекта'
};

async function loadPage(name) {
  if (!name) name = 'news';

  if (PAGE_TITLES[name]) pageTitle.textContent = PAGE_TITLES[name];

  contentArea.innerHTML = '<div class="loading">Загрузка...</div>';

  try {
    const res = await fetch('content/' + name + '.html');
    if (!res.ok) throw new Error('HTTP ' + res.status);
    const html = await res.text();
    contentArea.innerHTML = html;
    window.scrollTo({ top: 0, behavior: 'smooth' });
    attachPageHandlers(name);
  } catch (err) {
    console.error('Ошибка загрузки:', err);
    contentArea.innerHTML = `
      <div class="error-page">
        <h2>⚠️ Не удалось загрузить раздел</h2>
        <p>${name}.html — ${err.message}</p>
        <button onclick="loadPage('news')" class="btn">← На главную</button>
      </div>
    `;
  }
}

function attachPageHandlers(name) {
  // Форма отзыва в ЖИЗЕ
  const reviewForm = document.getElementById('review-form');
  if (reviewForm) reviewForm.addEventListener('submit', handleReviewSubmit);

  // Инициализация листьев на странице «Направления»
  if (name === 'directions' && typeof window.initLeaves === 'function') {
    window.initLeaves();
  }

  // Перезапуск анимаций
  if (typeof window.initScrollAnimations === 'function') {
    requestAnimationFrame(() => {
      window.initScrollAnimations();
    });
  }
}

function handleReviewSubmit(e) {
  e.preventDefault();
  const name    = document.getElementById('review-name').value;
  const message = document.getElementById('review-message').value;
  const status  = document.getElementById('review-status');

  console.log('Отзыв:', { name, message });
  status.textContent = '✅ Спасибо, ' + name + '! Отзыв отправлен.';
  status.className = 'review-status success';
  e.target.reset();

  setTimeout(() => {
    status.textContent = '';
    status.className = 'review-status';
  }, 5000);
}

document.querySelectorAll('.menu-item[data-page]').forEach(btn => {
  btn.addEventListener('click', () => {
    const page = btn.getAttribute('data-page');
    loadPage(page);

    document.querySelectorAll('.menu-item').forEach(m => m.classList.remove('active'));
    btn.classList.add('active');

    if (window.innerWidth < 1024) closeMenu();
  });
});

// ============================================================
//  Анимации при скролле — карточки выезжают при появлении
// ============================================================

const scrollObserver = new IntersectionObserver((entries) => {
  entries.forEach((entry, index) => {
    if (entry.isIntersecting) {
      // Задержка по очереди — 80ms
      setTimeout(() => {
        entry.target.classList.add('visible');
      }, index * 80);
      scrollObserver.unobserve(entry.target);
    }
  });
}, {
  threshold: 0.1,
  rootMargin: '0px 0px -40px 0px'
});

// Применить ко всем «анимируемым» элементам
function applyScrollAnimations() {
  const selectors = [
    '.news-card',
    '.team-card',
    '.stat-card',
    '.how-step',
    '.tech-card',
    '.direction-card',
    '.action-card',
    '.blog-post',
    '.hero',
    '.join-section',
    '.cta-section'
  ];

  selectors.forEach(sel => {
    document.querySelectorAll(sel).forEach(el => {
      if (!el.classList.contains('animate-on-scroll')) {
        el.classList.add('animate-on-scroll');
        scrollObserver.observe(el);
      }
    });
  });
}

// ============================================================
//  Анимация счётчика статистики
// ============================================================

function animateCounter(el) {
  const target = parseInt(el.getAttribute('data-target'), 10);
  if (isNaN(target)) return;

  const suffix = el.getAttribute('data-suffix') || '';
  const duration = 2500;             // миллисекунд
  const startTime = performance.now();

  function update(now) {
    const elapsed = now - startTime;
    const progress = Math.min(elapsed / duration, 1);

    // Плавное замедление в конце (easeOutQuart)
    const eased = 1 - Math.pow(1 - progress, 4);
    const current = Math.round(target * eased);

    el.textContent = current + suffix;

    if (progress < 1) {
      requestAnimationFrame(update);
    } else {
      el.textContent = target + suffix;
    }
  }

  requestAnimationFrame(update);
}

// Запуск при появлении в зоне видимости
const counterObserver = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      animateCounter(entry.target);
      counterObserver.unobserve(entry.target);
    }
  });
}, {
  threshold: 0.5
});

function initCounters() {
  document.querySelectorAll('.stat-value[data-target]').forEach(el => {
    // Сбросить на 0 перед стартом
    el.textContent = '0' + (el.getAttribute('data-suffix') || '');
    counterObserver.observe(el);
  });
}

// ============================================================
//  Загрузка страницы
// ============================================================

document.addEventListener('DOMContentLoaded', () => {
  loadPage('news');
  setTimeout(() => {
    applyScrollAnimations();
    initCounters();
  }, 300);
});

// Переопределить loadPage, чтобы счётчики запускались и на других страницах
if (typeof loadPage === 'function') {
  const originalFn = loadPage;
  window.loadPage = async function(name) {
    await originalFn(name);
    setTimeout(() => {
      applyScrollAnimations();
      initCounters();
    }, 200);
  };
}

// И после каждой загрузки раздела
const originalLoadPage = window.loadPage;
if (typeof loadPage === 'function') {
  const originalFn = loadPage;
  window.loadPage = async function(name) {
    await originalFn(name);
    setTimeout(applyScrollAnimations, 200);
  };
}
