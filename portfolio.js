const toggle=document.querySelector('.menu');
const links=document.querySelector('.navlinks');
toggle?.addEventListener('click',()=>{const open=toggle.getAttribute('aria-expanded')!=='true';toggle.setAttribute('aria-expanded',String(open));links?.classList.toggle('open',open);toggle.textContent=open?'Close':'Menu';});
links?.querySelectorAll('a').forEach(a=>a.addEventListener('click',()=>{links.classList.remove('open');toggle?.setAttribute('aria-expanded','false');if(toggle)toggle.textContent='Menu';}));
document.querySelector('[data-copy-email]')?.addEventListener('click',async()=>{const status=document.querySelector('.copy-status');try{await navigator.clipboard.writeText('hsetaknev7@gmail.com');status.textContent='Email copied. Let’s talk.';}catch{status.textContent='hsetaknev7@gmail.com';}});
