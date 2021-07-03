import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { SistArchComponent } from './sist-arch.component';

describe('SistArchComponent', () => {
  let component: SistArchComponent;
  let fixture: ComponentFixture<SistArchComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ SistArchComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(SistArchComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
