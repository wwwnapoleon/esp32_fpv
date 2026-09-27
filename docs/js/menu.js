// ============================================================
//  Боковое меню: открытие/закрытие, подпункты
// ============================================================

const sidebar  = document.getElementById('sidebar');
const overlay  = document.getElementById('sidebar-overlay');
const burger   = document.getElementById('burger');

function openMenu() {
  sidebar?.classList.add('open');
  overlay?.classList.add('visible');
  document.body.classList.add('menu-open');
}

function closeMenu() {
  sidebar?.classList.remove('open');
  overlay?.classList.remove('visible');
  document.body.classList.remove('menu-open');
}

burger?.addEventListener('click', () => {
  if (sidebar?.classList.contains('open')) closeMenu();
  else openMenu();
});

overlay?.addEventListener('click', closeMenu);

document.querySelectorAll('[data-toggle]').forEach(btn => {
  btn.addEventListener('click', () => {
    const target = btn.getAttribute('data-toggle');
    const submenu = document.getElementById('submenu-' + target);
    const arrow = btn.querySelector('.arrow');

    if (submenu) {
      const isOpen = submenu.classList.toggle('open');
      if (arrow) arrow.textContent = isOpen ? '▾' : '▸';
    }
  });
});

document.querySelectorAll('.submenu-item').forEach(link => {
  link.addEventListener('click', () => {
    if (window.innerWidth < 1024) closeMenu();
  });
});

document.addEventListener('keydown', (e) => {
  if (e.key === 'Escape') closeMenu();
});

// ============================================================
//  Автоскрытие бокового меню на десктопе
//  Если курсор уходит с меню — плашка закрывается
// ============================================================

// Работает только на десктопе (ширина ≥ 1024px)
const isDesktop = () => window.innerWidth >= 1024;

// Если меню открыто и курсор ушёл с него — закрываем через 600мс
let autoCloseTimer = null;

sidebar?.addEventListener('mouseleave', () => {
  if (!isDesktop()) return;                 // на мобильном — не трогаем
  if (!sidebar.classList.contains('open')) return;  // если закрыто — ничего

  // Небольшая задержка — чтобы не закрывать, если курсор
  // случайно «чиркнул» по краю
  autoCloseTimer = setTimeout(() => {
    closeMenu();
  }, 600);
});

// Если курсор вернулся — отменяем закрытие
sidebar?.addEventListener('mouseenter', () => {
  if (autoCloseTimer) {
    clearTimeout(autoCloseTimer);
    autoCloseTimer = null;
  }
});
